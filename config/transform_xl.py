import xlrd
import re
import string
import numpy as np
import copy


connect_filename = "noi_connection_for_sw_0302.xlsx"
# IO_filename = "IO.xlsx"
#filename_pin_position = "Bump_PHY_Plcae.xlsx"
outfilename = "test_noi.txt"




class muyan_index():
    die_num = 0
    cpu_num = 0
    cpu_pin_num = 0
    pin_num = 0
    chip = []
    chip_flag = []
    cpu = []
    pos_and_neg = []
    pos_and_neg_num = 0
    pin_position = []
    xinzhaiIO_position = []
    num = "0 0 0 0 0"
    Frequence = 0
    die_index = [[8, 0], [8, 3], [8, 6], [8, 9],
                  [6, 0], [6, 3], [6, 6], [6, 9],
                  [4, 0], [4, 3], [4, 6], [4, 9],
                  [2, 0], [2, 3], [2, 6], [2, 9], ]
    pin_index = [[0, 0], [0, 1], [0, 2], [0, 3], [0, 4], [0, 5], [0, 6], [0, 7], [0, 8], [0, 9], [0, 10], [0, 11], [0, 12], [0, 13], [0, 14], [0, 15],
                 [1, 0], [1, 1], [1, 2], [1, 3], [1, 4], [1, 5], [1, 6], [1, 7], [1, 8], [1, 9], [1, 10], [1, 11], [1, 12], [1, 13], [1, 14], [1, 15],
                 [2, 0], [2, 1], [2, 2], [2, 3], [2, 4], [2, 5], [2, 6], [2, 7], [2, 8], [2, 9], [2, 10], [2, 11], [2, 12], [2, 13], [2, 14], [2, 15],
                 [3, 0], [3, 1], [3, 2], [3, 3], [3, 4], [3, 5], [3, 6], [3, 7], [3, 8], [3, 9], [3, 10], [3, 11], [3, 12], [3, 13], [3, 14], [3, 15],
                 [4, 0], [4, 1], [4, 2], [4, 3], [4, 4], [4, 5], [4, 6], [4, 7], [4, 8], [4, 9], [4, 10], [4, 11], [4, 12], [4, 13], [4, 14], [4, 15],
                 [5, 0], [5, 1], [5, 2], [5, 3], [5, 4], [5, 5], [5, 6], [5, 7], [5, 8], [5, 9], [5, 10], [5, 11], [5, 12], [5, 13], [5, 14], [5, 15],
                 [6, 0], [6, 1], [6, 2], [6, 3], [6, 4], [6, 5], [6, 6], [6, 7], [6, 8], [6, 9], [6, 10], [6, 11], [6, 12], [6, 13], [6, 14], [6, 15],
                 [7, 0], [7, 1], [7, 2], [7, 3], [7, 4], [7, 5], [7, 6], [7, 7], [7, 8], [7, 9], [7, 10], [7, 11], [7, 12], [7, 13], [7, 14], [7, 15]]
    cpu_index = [[0, 9, 4], [0, 0, 5], [0, 9, 7], [1, 3, 12],[0, 0, 7]]

    def __init__(self, num):
        self.die_num = num
        for i in range(num):
            self.chip.append([])
        for i in range(9):
            self.cpu_pin_num = 0
        for i in range(num):
            self.chip_flag.append(0)

    def insertdie(self, str):
        matchobj = re.match(r'xinzhai_muyan_([0-9]*)_.*' , str)
        if matchobj:
            self.die_num = int(matchobj.group(1))
            flag = 0
            for i in range(len(self.chip[self.die_num])):
                if str == self.chip[self.die_num][i][0]:
                    flag = 1
            if flag == 0:
                self.chip[self.die_num].append([str, self.die_num, self.chip_flag[self.die_num]])
                self.chip_flag[self.die_num] += 1
        else:
            matchobj = re.match(r'i_cpu_onfiHost_([0-9]*)_data_([0-9]*)', str)
            if matchobj:
                self.cpu_num = int(matchobj.group(1))
                flag = 0
                for i in range(len(self.cpu[self.cpu_num])):
                    if str == self.cpu[self.cpu_num][i][0]:
                        flag = 1
                if flag == 0:
                    self.cpu[self.cpu_num].append([str, self.cpu_num, self.cpu_pin_num[self.cpu_num]])
                    self.cpu_pin_num[self.cpu_num] += 1
            else:
                matchobj = re.match(r'i_cpu_onfiHost_([0-9]*)_.*', str)
                if matchobj:
                    self.cpu_num = int(matchobj.group(1))
                    flag = 0
                    for i in range(len(self.cpu[self.cpu_num])):
                        if str == self.cpu[self.cpu_num][i][0]:
                            flag = 1
                    if flag == 0:
                        self.cpu[self.cpu_num].append([str, self.cpu_num, self.cpu_pin_num[self.cpu_num]])
                        self.cpu_pin_num[self.cpu_num] += 1
                else:
                    matchobj = re.match(r'i_(.*)_gpio_([0-9]*)', str)
                    if matchobj:
                        if matchobj.group(1) == 'cpu':
                            if(int(matchobj.group(2)) < 16):
                                self.cpu[4].append([str, 4, int(matchobj.group(2))])
                            else:
                                self.cpu[5].append([str, 5, int(matchobj.group(2)) - 16])
                        elif matchobj.group(1) == 'xinzhai':
                            if (int(matchobj.group(2)) < 16):
                                self.cpu[6].append([str, 6, int(matchobj.group(2))])
                            else:
                                self.cpu[7].append([str, 7, int(matchobj.group(2)) - 16])
                    else:
                        matchobj = re.match(r'i_xinzhai_(.*)',  str)
                        if matchobj:
                            if matchobj.group(1) == 'nege':
                                flag = 0
                                for i in range(len(self.pos_and_neg)):
                                    if str == self.pos_and_neg[i][0]:
                                        flag = 1
                                if flag == 0:
                                    self.pos_and_neg.append([str, -1])
                                    self.pos_and_neg_num += 1
                            elif matchobj.group(1) == 'pose':
                                flag = 0
                                for i in range(len(self.pos_and_neg)):
                                    if str == self.pos_and_neg[i][0]:
                                        flag = 1
                                if flag == 0:
                                    self.pos_and_neg.append([str, -2])
                                    self.pos_and_neg_num += 1
                            else:
                                matchobj2 = re.match(r'.*_csn_([0-9]*)' , matchobj.group(1))
                                if matchobj2:
                                    self.cpu[3].append([str, 4, matchobj2.group(1)])
                                else:
                                    flag = 0
                                    for i in range(len(self.cpu[8])):
                                        if str == self.cpu[8][i][0]:
                                            flag = 1
                                    if flag == 0:
                                        self.cpu[8].append([str, 8, self.cpu_pin_num[8]])
                                        self.cpu_pin_num[8] += 1

    def get_num(self, str1):
        matchobj = re.match(r'muyan_([0-9]*)_(.*)', str1)
        if matchobj:
            flag = -1
            self.die_num = int(matchobj.group(1))
            for i in range(len(self.pin_position)):
                if matchobj.group(2) == self.pin_position[i][0]:
                    flag = i
                    break

            self.num = "0" + " " + str(self.die_index[self.die_num][0]) + " " + str(self.die_index[self.die_num][1]) + " " + str(int(self.pin_position[flag][2])%8)+ " " + str(int(self.pin_position[flag][1])*8+int(int(self.pin_position[flag][2])/8))
        else:
            matchobj = re.match(r'xinzhai_nege', str1)  # 
            if matchobj:
                self.num = "0" + " " + "0" + " " + "0" + " " + "-1" + " " + "0"     # 随便连一个外部IO端口, GND
            else:
                matchobj = re.match(r'xinzhai_pose',str1) # 
                if matchobj:
                    self.num = "0" + " " + "0" + " " + "0" + " " + "-2" + " " + "0"     # 随便连一个外部IO端口, POWER
                else:
                    flag = -1
                    for i in range(len(self.xinzhaiIO_position)):
                        if str1 == self.xinzhaiIO_position[i][0]:
                            flag = i
                    self.num = self.xinzhaiIO_position[flag][1] + " " + "-3" + " " +str(int(self.xinzhaiIO_position[flag][2]))
    


    def reset(self):
        self.pin_num = 0

    def load_xlsx(self, filename):
        data = xlrd.open_workbook(filename)
        table = data.sheet_by_index(0)
        for i in range(len(table.col(1))):
            if table.cell(i, 1).ctype != 0:
                self.insertdie(table.cell(i, 1).value)
            else:
                self.reset()
        for i in range(len(table.col(2))):
            if table.cell(i, 0).ctype != 0:
                self.insertdie(table.cell(i, 0).value)

    def get_Frequence(self, str):
        matchobj = re.match(r"\D+([0-9]+)MHz", str)
        if matchobj:
            self.Frequence = matchobj.group(1)
        else:
            self.Frequence = 0

    def write_txt(self, connect_file, filename1):
        data = xlrd.open_workbook(connect_file)
        table = data.sheet_by_index(1)
        fileobject = open(filename1, 'w')
        for i in range(len(table.col(0))):
            if i == 0:
                continue
            else:
                if table.cell(i, 1).ctype != 0:
                    self.get_num(table.cell(i, 0).value)
                    fileobject.write(self.num)
                    fileobject.write('\t')
                    self.get_num(table.cell(i, 1).value)
                    fileobject.write(self.num)
                    fileobject.write('\t')
                    fileobject.write(str(int(table.cell(i, 2).value)))
                    fileobject.write('\n')
                else:
                    fileobject.write('\n')
        fileobject.close()

    def obtain_pin_position(self, filename):
        data = xlrd.open_workbook(filename)
        table = data.sheet_by_index(0)
        for i in range(len(table.col(0))):
            if i == 0:
                continue
            else:
                matchobj = re.match(r".*([0-9]+)_([0-9]+)", table.cell(i, 0).value)
                if (matchobj and (table.cell(i,1).value!="")):
                    self.pin_position.append([table.cell(i,1).value,matchobj.group(1),matchobj.group(2)])

    def obtain_xinzhaiIO_position(self, IO_filename):
        data = xlrd.open_workbook(IO_filename)
        table = data.sheet_by_index(2)
        for i in range(len(table.col(0))):
            if((table.cell(i,1).value!="")):
                self.xinzhaiIO_position.append([table.cell(i,0).value,table.cell(i,1).value,table.cell(i,2).value])

a = muyan_index(16)
a.obtain_pin_position(connect_filename)
a.obtain_xinzhaiIO_position(connect_filename)
a.write_txt(connect_filename, outfilename)
s = 1


