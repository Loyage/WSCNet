import numpy as np 
import pandas as pd
import cv2
#from sklearn.externals import joblib
import time
import re
import argparse
from PIL import Image
import requests
from concurrent import futures

import json
import urllib.request, sys
import base64

def sdk_response(param):
    gray_img = param['gray_img']
    _dropCircle = param['dropCircle']
    port = param['port']

    _circle = [int(item+0.5) for item in _dropCircle[0:3]]#circle 圆心和半径
    _label = int(_dropCircle[3])  #类别标签

    sp = gray_img.shape #图像分辨率
    img_height = sp[0]
    img_width = sp[1]

    rect_tl = [_circle[0] - _circle[2]-1, _circle[1] - _circle[2]-1]
    rect_br = [_circle[0] + _circle[2], _circle[1] + _circle[2]]

    real_tl = np.maximum(rect_tl,0)
    real_br = [rect_br[0] if rect_br[0] < img_width else img_width-1, 
            rect_br[1] if rect_br[1] < img_height else img_height-1]
    
    _rect = np.append(real_tl,[real_br[0]-real_tl[0],real_br[1]-real_tl[1]])

    roi_img = gray_img[real_tl[1]:real_br[1],real_tl[0]:real_br[0]]
    
    #new_dim = 50
    #roi_img = cv2.resize(roi_img,(new_dim,new_dim))

    ret,buf = cv2.imencode('.bmp',roi_img)
    img_bytes = Image.fromarray(np.uint8(buf)).tobytes()
    response = requests.post('http://127.0.0.1:'+str(port)+'/', params={'threshold': 0.1},data=img_bytes).json()

    drop_result = response['results'][0]
    drop_confidence = drop_result['confidence']
    drop_class = drop_result['index']-1

    return drop_class


if  __name__ == "__main__":

    # 命令行传递参数（检测结果txt文件名）
    
    parser = argparse.ArgumentParser() 
    # parser.add_argument('port', type = str, help = 'Port')
    parser.add_argument('img_address', type = str, help = 'Image Address')
    parser.add_argument('label_address', type = str, help = 'Label Address')
    args = parser.parse_args()

    # current_port = args.port
    current_img_address = args.img_address
    current_label_address = args.label_address
    

    # current_img_address = 'E:\\share\\Droplet_Data\\0814\\2\\js\\72.bmp'
    # current_label_address = 'E:\\share\\Droplet_Data\\0814\\2\\js\\dropInformation\\72.bmp.txt'

    print(current_img_address)
    print(current_label_address)

    src_img = cv2.imread(current_img_address)
    gray_img = cv2.cvtColor(src_img,cv2.COLOR_BGR2GRAY)

    dropCircles = np.loadtxt(current_label_address)

    # request_root = "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/classification/droplet_detection"
    # access_token = '24.e6ab299e31319a22dea3630a78352de5.2592000.1567658196.282335-16937528'
    # request_url = request_root + "?access_token=" + access_token

    params = []
    count = 0
    for _dropCircle in dropCircles:
        port = count%6 + 24401
        params.append({ 
            'port':port,
            'gray_img':gray_img,
            'dropCircle':_dropCircle,
            })
        count = count+1

        # test = sdk_response({ 
        #     'port':port,
        #     'gray_img':gray_img,
        #     'dropCircle':_dropCircle,
        #     })
        
        #api结果
        # bs64img = str(base64.b64encode(img_bytes), "utf-8")
        # params = {'image':bs64img,'top_num':5}
        # header = {"Content-Type": "application/json"}
        # response = requests.post(request_url,
        #     data=json.dumps(params),
        #     headers=header
        #     )
        # s = str(response.content, "utf-8")
        # print(s)

        #cv2.imshow('roi',roi_img)
        #cv2.waitKey()

    
    t_start= time.time()
    #多进程处理
    workers = 6
    with futures.ProcessPoolExecutor(workers) as executor:
        res_classes = executor.map(sdk_response,params)
    
    drop_classes = list(res_classes)
    print(time.time()-t_start)

    # 保存圆形多分类结果
    with open(current_label_address+'.txt','w') as circle_label:
        for j in range(0,len(drop_classes)):
            circle_label.write(f'{dropCircles[j,0]} {dropCircles[j,1]} {dropCircles[j,2]} {drop_classes[j]}\n')
        circle_label.close()



    # save_rect_label_address = test_detlabel_address + current_img_name + '_object.txt'
    # save_score_label_address = test_detlabel_address + current_img_name + '_multiclass.txt'
    # save_circle_label_address = test_detlabel_address + current_img_name + '_circle.txt'
    
    # #   保存矩形多分类结果
    # with open(save_score_label_address,'w') as score_label:
    #     for j in range(0,len(test_predictions)):
    #         if(test_predictions[j]==0):
    #             continue
    #         elif(test_predictions[j]==1):
    #             _label = 'empty'
    #         elif(test_predictions[j]==2):
    #             _label = 'single'
    #         elif(test_predictions[j]==3):
    #             _label = 'multiple'
    #         _score = probs[j,test_predictions[j]]

    #         rect = []
    #         rect.append(int(dets[j,0] - dets[j,2] + 0.5))
    #         rect.append(int(dets[j,1] - dets[j,2] + 0.5))
    #         rect.append(int(dets[j,2]*2 + 0.5))
    #         rect.append(int(dets[j,2]*2 + 0.5))

    #         score_label.write(f'{_label} {_score} {rect[0]} {rect[1]} {rect[2]} {rect[3]}\n')
    #     score_label.close()

    # #  保存矩形检测结果
    # with open(save_rect_label_address,'w') as rect_label:
    #     for j in range(0,len(test_predictions)):
    #         if(test_predictions[j]==0):
    #             continue
    #         _score = probs[j,test_predictions[j]]
    #         rect = []
    #         rect.append(int(dets[j,0] - dets[j,2] + 0.5))
    #         rect.append(int(dets[j,1] - dets[j,2] + 0.5))
    #         rect.append(int(dets[j,2]*2 + 0.5))
    #         rect.append(int(dets[j,2]*2 + 0.5))
    #         rect_label.write(f'droplet {_score} {rect[0]} {rect[1]} {rect[2]} {rect[3]}\n')
    #     rect_label.close()

    # # 保存圆形多分类结果
    # with open(save_circle_label_address,'w') as circle_label:
    #     for j in range(0,len(test_predictions)):
    #         if(test_predictions[j]==0):
    #             continue
    #         elif(test_predictions[j]==1):
    #             _label = 'empty'
    #         elif(test_predictions[j]==2):
    #             _label = 'single'
    #         elif(test_predictions[j]==3):
    #             _label = 'multiple'
    #         _score = probs[j,test_predictions[j]]

    #         circle_label.write(f'{_label} {_score} {dets[j,0]} {dets[j,1]} {dets[j,2]}\n')
    #     circle_label.close()