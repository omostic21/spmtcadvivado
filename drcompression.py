import numpy as np
import matplotlib.pyplot as plt
import os
from collections import OrderedDict
import logging
import collections
import operator

import write_to_BIN
import util
import data
import config
import time
import copy

target_folder = config.target_folder
benchmark_folder = config.benchmark_folder


def update_result(result: dict, N, M, val, rid, ucids, map: list):

    assert len(map) == M + N + 1
    assert len(val) == M + N
    assert len(rid) == M + N
    assert len(ucids) == M

    while len(map) < 7:
        map.insert(len(map) - 1, 3)

    # N = 4
    # M = 2
    #
    # while len(val) < 6:
    #     val.append(1.0)
    #
    # while len(rid) < 6:
    #     rid.append(31)
    #
    # while len(ucids) < 2:
    #     ucids.append(config.pad_cid)

    result['N'] = N
    result['M'] = M
    result['val'] = val
    result['rid'] = rid
    result['ucids'] = ucids
    result['map'] = map


def compression(all_data: OrderedDict, row_batch: int, path: str, process_freq_dict: dict):
    # the new compressed format: 512 bits per group
    # bitmap: 3 bits: N -- number of reused elements up to 4
    #         1 bits: M -- number of unreused elements up to 2
    #         (N+M)*64 bits -- value segment
    #         (N+M)*5 bits -- rid segment
    #         M*32 -- unbuffered cids
    #         (N+M)*2 + 1 -- vector map and update
    # map : 0-cid1, 1-cid2, 2-rcid1

    column_length = all_data[row_batch]['column_length']
    column_len = OrderedDict(sorted(column_length.items(), key=operator.itemgetter(1), reverse=True))
    element_counter = all_data[row_batch]['element_counter']
    elements_cid_order = all_data[row_batch]['elements_cid_order']

    terminate = -1
    reused_cid1 = terminate
    reused_length1 = 0

    result_dict = OrderedDict()
    curr_group_idx = 0

    longest_column = list(column_len.keys())[0]
    longest_len = column_len.pop(longest_column)

    if column_len:
        shortest_column = list(column_len.keys())[-1]
        shortest_len = column_len.pop(shortest_column)
    else:
        shortest_column = terminate
        shortest_len = 0

    while element_counter > 0:
        # if no buffered cid
        if reused_cid1 == terminate:
            if longest_column == terminate or shortest_column == terminate:
                unique_column = longest_column if shortest_column == terminate else shortest_column
                unique_len = len(elements_cid_order[unique_column])
                for _ in range(unique_len // 6):
                    val = [1.0] * 6
                    N = 5
                    M = 1
                    map = [0] * 6 + [0]
                    ucids = [unique_column]
                    rid = elements_cid_order[unique_column][:6]
                    result_dict[curr_group_idx] = dict()
                    update_result(result_dict[curr_group_idx], N, M, val, rid, ucids, map)
                    curr_group_idx += 1
                    for _ in range(6):
                        elements_cid_order[unique_column].pop(0)
                    element_counter -= 6
                unique_len = unique_len % 6
                if unique_len > 0:
                    val = [1.0] * unique_len
                    N = unique_len - 1
                    M = 1
                    map = [0] * unique_len + [0]
                    ucids = [unique_column]
                    rid = elements_cid_order[unique_column][:6]
                    result_dict[curr_group_idx] = dict()
                    update_result(result_dict[curr_group_idx], N, M, val, rid, ucids, map)
                    curr_group_idx += 1
                    for _ in range(unique_len):
                        elements_cid_order[unique_column].pop(0)
                    element_counter -= unique_len
            else:
                # if the first one can fulfill the bandwidth
                if longest_len + shortest_len > 6:
                    operate_length = 5 if shortest_len >= 6 else shortest_len
                    val = [1.0] * 6
                    N = 4
                    M = 2
                    map = [0] * (6 - operate_length) + [1] * operate_length + [1]
                    ucids = [longest_column, shortest_column]
                    rid = elements_cid_order[longest_column][: (6-operate_length)] + \
                          elements_cid_order[shortest_column][: operate_length]
                    result_dict[curr_group_idx] = dict()
                    update_result(result_dict[curr_group_idx], N, M, val, rid, ucids, map)
                    curr_group_idx += 1
                    for _ in range(6 - operate_length):
                        elements_cid_order[longest_column].pop(0)
                    for _ in range(operate_length):
                        elements_cid_order[shortest_column].pop(0)
                        shortest_len -= 1
                    element_counter -= 6
                    longest_len -= (6 - operate_length)
                    if shortest_len == 0:
                        if column_len:
                            shortest_column = list(column_len.keys())[-1]
                            shortest_len = column_len.pop(shortest_column)
                        else:
                            shortest_column = terminate
                            shortest_len = 0
                    reused_cid1 = longest_column
                    reused_length1 = longest_len
                else:
                    val = [1.0] * (longest_len + shortest_len)
                    N = longest_len + shortest_len - 2
                    M = 2
                    map = [0] * longest_len + [1] * shortest_len + [0]
                    ucids = [longest_column, shortest_column]
                    rid = elements_cid_order[longest_column][:] + elements_cid_order[shortest_column][:]
                    result_dict[curr_group_idx] = dict()
                    update_result(result_dict[curr_group_idx], N, M, val, rid, ucids, map)
                    curr_group_idx += 1
                    elements_cid_order[longest_column] = []
                    elements_cid_order[shortest_column] = []
                    element_counter -= (longest_len + shortest_len)
                    if column_len:
                        longest_column = list(column_len.keys())[0]
                        longest_len = column_len.pop(longest_column)
                    else:
                        longest_column = terminate
                        longest_len = 0
                    if column_len:
                        shortest_column = list(column_len.keys())[-1]
                        shortest_len = column_len.pop(shortest_column)
                    else:
                        shortest_column = terminate
                        shortest_len = 0
        else:
            if shortest_column == terminate:
                unique_column = reused_cid1
                unique_len = len(elements_cid_order[unique_column])
                for _ in range(unique_len // 6):
                    val = [1.0] * 6
                    N = 5
                    M = 1
                    map = [0] * 6 + [0]
                    ucids = [unique_column]
                    rid = elements_cid_order[unique_column][:6]
                    result_dict[curr_group_idx] = dict()
                    update_result(result_dict[curr_group_idx], N, M, val, rid, ucids, map)
                    curr_group_idx += 1
                    for _ in range(6):
                        elements_cid_order[unique_column].pop(0)
                    element_counter -= 6
                unique_len = unique_len % 6
                if unique_len > 0:
                    val = [1.0] * unique_len
                    N = unique_len - 1
                    M = 1
                    map = [0] * unique_len + [0]
                    ucids = [unique_column]
                    rid = elements_cid_order[unique_column][:6]
                    result_dict[curr_group_idx] = dict()
                    update_result(result_dict[curr_group_idx], N, M, val, rid, ucids, map)
                    curr_group_idx += 1
                    for _ in range(unique_len):
                        elements_cid_order[unique_column].pop(0)
                    element_counter -= unique_len
            else:
                if reused_length1 + shortest_len >= 6:
                    operate_length = 5 if shortest_len >= 6 else shortest_len
                    val = [1.0] * 6
                    N = 5
                    M = 1
                    map = [2] * (6 - operate_length) + [0] * operate_length + [0]
                    ucids = [shortest_column]
                    rid = elements_cid_order[reused_cid1][: (6-operate_length)] + \
                          elements_cid_order[shortest_column][: operate_length]
                    result_dict[curr_group_idx] = dict()
                    update_result(result_dict[curr_group_idx], N, M, val, rid, ucids, map)
                    curr_group_idx += 1
                    for _ in range(6 - operate_length):
                        elements_cid_order[reused_cid1].pop(0)
                    for _ in range(operate_length):
                        elements_cid_order[shortest_column].pop(0)
                        shortest_len -= 1
                    element_counter -= 6
                    reused_length1 -= (6 - operate_length)
                    if reused_length1 == 0:
                        reused_cid1 = terminate
                        if column_len:
                            longest_column = list(column_len.keys())[0]
                            longest_len = column_len.pop(longest_column)
                        else:
                            longest_column = terminate
                            longest_len = 0
                    if shortest_len == 0:
                        if column_len:
                            shortest_column = list(column_len.keys())[-1]
                            shortest_len = column_len.pop(shortest_column)
                        else:
                            shortest_column = terminate
                            shortest_len = 0
                else:
                    pre_shortest_column = shortest_column
                    pre_shortest_len = shortest_len
                    if column_len:
                        shortest_column = list(column_len.keys())[-1]
                        shortest_len = column_len.pop(shortest_column)
                    else:
                        shortest_column = terminate
                        shortest_len = 0

                    total_len = reused_length1 + shortest_len + pre_shortest_len
                    if shortest_column == terminate:
                        N = total_len - 1
                        M = 1
                        map = [2] * reused_length1 + [0] * pre_shortest_len + [0]
                        val = (N + M) * [1.0]
                        rid = elements_cid_order[reused_cid1][:] + \
                              elements_cid_order[pre_shortest_column][:]
                        ucids = [pre_shortest_column]
                    else:
                        N = 4 if total_len > 6 else total_len - 2
                        M = 2
                        map = [2] * reused_length1 + [0] * pre_shortest_len + \
                              [1] * (N + M - reused_length1 - pre_shortest_len) + [0]
                        val = (N + M) * [1.0]
                        rid = elements_cid_order[reused_cid1][:] + \
                              elements_cid_order[pre_shortest_column][:] + \
                              elements_cid_order[shortest_column][: (N + M - reused_length1 - pre_shortest_len)]
                        ucids = [pre_shortest_column, shortest_column]
                    result_dict[curr_group_idx] = dict()
                    update_result(result_dict[curr_group_idx], N, M, val, rid, ucids, map)
                    curr_group_idx += 1
                    element_counter -= N + M
                    elements_cid_order[reused_cid1] = []
                    elements_cid_order[pre_shortest_column] = []
                    if shortest_column != terminate:
                        for _ in range(N + M - reused_length1 - pre_shortest_len):
                            elements_cid_order[shortest_column].pop(0)
                            shortest_len -= 1
                    reused_cid1 = terminate
                    reused_length1 = 0
                    if column_len:
                        longest_column = list(column_len.keys())[0]
                        longest_len = column_len.pop(longest_column)
                    else:
                        longest_column = terminate
                        longest_len = 0
                    if shortest_len == 0:
                        if column_len:
                            shortest_column = list(column_len.keys())[-1]
                            shortest_len = column_len.pop(shortest_column)
                        else:
                            shortest_column = terminate
                            shortest_len = 0

    # with open(path, 'a') as wfile:
    #     wfile.write("{}\n".format(curr_group_idx))
    #     for value in result_dict.values():
    #         target_str = ""
    #         target_str += "{} {}".format(value['N'], value['M'])
    #         target_str += ' '
    #         for val in value['val']:
    #             target_str += "{}".format(val)
    #             target_str += ' '
    #         for rid in value['rid']:
    #             target_str += "{}".format(rid)
    #             target_str += ' '
    #         for ucid in value['ucids']:
    #             target_str += "{}".format(ucid)
    #             target_str += ' '
    #         for map in value['map']:
    #             target_str += "{}".format(map)
    #             target_str += ' '
    #         target_str += '\n'
    #         wfile.write(target_str)

    # total_bit = 0
    #
    # for value in result_dict.values():
    #     total_bit += 3 + 2 + (value['N'] + value['M']) * 71 + 1 + value['M'] * 32

    intra_reuse = 0
    inter_reuse = 0

    for value in result_dict.values():
        intra_1_found = False
        intra_2_found = False
        value['map'].pop(len(value['map']) - 1)
        for item in value['map']:
            if item == 2:
                inter_reuse += 1
            elif item == 0:
                if not intra_1_found:
                    intra_1_found = True
                else:
                    intra_reuse += 1
            elif item == 1:
                if not intra_2_found:
                    intra_2_found = True
                else:
                    intra_reuse += 1
        if value['N'] + value['M'] >= 4:
            process_freq_dict[value['N'] + value['M']] += value['N'] + value['M']

    return intra_reuse, inter_reuse


def main():
    if not os.path.exists(os.getcwd() + "/" + target_folder):
        os.mkdir(target_folder)

    if not os.path.exists(os.getcwd() + "/" + config.target_bin_folder):
        os.mkdir(config.target_bin_folder)

    for tfile in os.listdir(benchmark_folder):
        # the unzipped benchmark folder
        if os.path.isdir(benchmark_folder + "/" + tfile):
            if os.path.exists(target_folder + "/" + tfile + ".txt"):
                os.remove(target_folder + "/" + tfile + ".txt")
            s_path = benchmark_folder + "/{}/{}.mtx".format(tfile, tfile)
            rid, cid, val, rows = data.initialize_matrix_data(s_path)
            
            if not os.path.exists(target_folder + "/" + tfile + "/"):
                os.mkdir(target_folder + "/" + tfile + "/")

            print("----------------------------------")
            print("{} has {} rows, {} nnzs".format(tfile, rows, len(val)))
            # row_batch数量
            total_batch = rows // config.rows if rows % config.rows == 0 else rows // config.rows + 1
            
            # print("CSR size {} b".format((rows+1)*32+len(val)*64+len(cid)*32))

            process_freq_dict = {4: 0, 5: 0, 6: 0}

            all_data = OrderedDict()
            # read elements to all the row-batches
            for i in range(total_batch):
                all_data[i] = {}
                all_data[i]['elements_cid_order'] = OrderedDict()
                all_data[i]['elements_rid_order'] = OrderedDict()
                all_data[i]['column_length'] = OrderedDict()
                all_data[i]['row_length'] = OrderedDict()
                all_data[i]['element_counter'] = 0

            for i in range(len(rid)):
                # 更新统计信息
                # 列主序的格式为 cid : list(rids) 行主序的为 rid: list(cids)
                # 长度统计格式均为 cid/rid : 次数
                util.map_dict_add(all_data[rid[i]//config.rows]['elements_cid_order'], {cid[i]: {rid[i]: val[i]}})
                util.map_dict_add(all_data[rid[i]//config.rows]['elements_rid_order'], {rid[i]: {cid[i]: 0}})
                util.counter_dict_add(all_data[rid[i]//config.rows]['column_length'], cid[i])
                util.counter_dict_add(all_data[rid[i]//config.rows]['row_length'], rid[i])
                all_data[rid[i]//config.rows]['element_counter'] += 1

            inter_reuse = 0
            intra_reuse = 0

            print("Begin -- {}".format(time.asctime(time.localtime())))
            for i in range(total_batch):
                # print("the {}th r_batch of {}, total process {:.2f}\n".format(i+1, tfile, (i+1)/total_batch))
                t_intra, t_inter = compression(all_data, i, target_folder + '/' + tfile + '/' + tfile + '.txt',
                                               process_freq_dict)
                inter_reuse += t_inter
                intra_reuse += t_intra
            print("Finished -- {}".format(time.asctime(time.localtime())))
            print("DRC inter {}".format(inter_reuse))
            print("DRC intra {}".format(intra_reuse))

            print(process_freq_dict)

            print("----------------------------------")


if __name__ == '__main__':
    main()

