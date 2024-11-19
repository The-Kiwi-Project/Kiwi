import openpyxl
import json

def xlsx_to_json(input_file, output_file):
    # 打开 Excel 文件
    workbook = openpyxl.load_workbook(input_file)
    sheet = workbook.active  # 获取第一个工作表
    
    connections = {}

    # 遍历工作表的每一行
    if sheet:
        for row in sheet.iter_rows(min_row=1, values_only=True):
            # 检查是否是空行（全为空）
            if not any(row):
                continue
            
            # 获取前三列的值
            string1 = row[0]  # 第一列
            string2 = row[1]  # 第二列
            number = row[2]   # 第三列

            # 检查数据是否有效（确保有值）
            if isinstance(string1, str) and isinstance(string2, str) and isinstance(number, (int, float)):
                # 将数字转换为字符串作为键
                number_str = str(int(number))
                # 添加到 connections 中
                if number_str not in connections:
                    connections[number_str] = [[string1, string2]]
                else:
                    connections[number_str].append([string1, string2])
    else:
        print("open sheet failed")
        exit(-1)
    
    # 写入 JSON 文件
    with open(output_file, 'w', encoding='utf-8') as json_file:
        json.dump(connections, json_file, indent=4, ensure_ascii=False)

    print(f"JSON 文件已保存到 {output_file}")


if __name__ == "__main__":
    input_xlsx = "config/muyan/connections.xlsx"  
    output_json = "config/muyan/connections.json"  

    xlsx_to_json(input_xlsx, output_json)
