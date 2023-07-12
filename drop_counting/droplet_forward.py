from __future__ import print_function
import torch as t
import torchvision as tv
import torchvision.transforms as transforms
import matplotlib.pyplot as plt
import numpy as np
import cv2
from torch.utils import data
import argparse
import time


#构建网络
import torch.nn as nn
import torch.nn.functional as F

class Net(nn.Module):
    def __init__(self):
        super(Net,self).__init__()
        self.conv1 = nn.Conv2d(3,6,5)
        self.conv2 = nn.Conv2d(6,16,5)
        self.fc1 = nn.Linear(16*5*5,120)
        self.fc2 = nn.Linear(120,84)
        self.fc3 = nn.Linear(84,4)
        
    def forward(self,x):
        x = F.max_pool2d(F.relu(self.conv1(x)),(2,2))
        x = F.max_pool2d(F.relu(self.conv2(x)),2)
        x = x.view(x.size()[0],-1)
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = self.fc3(x)
        return x
 

class dropletData(data.Dataset):
    def __init__(self, img_root,img_name,nsize = (32,32),transforms=None):
        img_path = img_root+img_name
        self.img = cv2.imread(img_path)
        _label_address = img_root+'dropInformation\\'+img_name+'.txt'
        self.dropCircles = np.loadtxt(_label_address)
        self.nsize = nsize
        self.transforms = transforms
    
    def __getdrop__(self,_dropCircle):
        _img = self.img

        _circle = [int(item+0.5) for item in _dropCircle[0:3]]#circle 圆心和半径

        sp = _img.shape #图像分辨率
        img_height = sp[0]
        img_width = sp[1]

        rect_tl = [_circle[0] - _circle[2]-1, _circle[1] - _circle[2]-1]
        rect_br = [_circle[0] + _circle[2], _circle[1] + _circle[2]]

        real_tl = np.maximum(rect_tl,0)
        real_br = [rect_br[0] if rect_br[0] < img_width else img_width-1, 
                rect_br[1] if rect_br[1] < img_height else img_height-1]

        roi_img = _img[real_tl[1]:real_br[1],real_tl[0]:real_br[0],:]
        
        drop_img = cv2.resize(roi_img,self.nsize)

        return drop_img

    def __getitem__(self, index):
        _dropimg = self.__getdrop__(self.dropCircles[index])
        # _dropimg = Image.fromarray(np.uint8(_dropimg)) #numpy转Image
        # data = t.from_numpy(_dropimg)
        if self.transforms:
            data = self.transforms(_dropimg)
        else:
            data = t.from_numpy(_dropimg)
        return data
    
    def __len__(self):
        return len(self.dropCircles)


if __name__ == '__main__':
    #获取数据

    parser = argparse.ArgumentParser() 
    # parser.add_argument('port', type = str, help = 'Port')
    parser.add_argument('img_address', type = str, help = 'Images Address')
    parser.add_argument('img_name', type = str, help = 'Image Name')
    args = parser.parse_args()

    img_dir = args.img_address
    img_name = args.img_name

    transform = transforms.Compose([
        transforms.ToTensor(), # 将图片(Image)转成Tensor，归一化至[0, 1]
        transforms.Normalize(mean=[.5, .5, .5], std=[.5, .5, .5]) # 标准化至[-1, 1]，规定均值和标准差
    ])

    dropletData_test = dropletData(img_dir,img_name,transforms=transform)

    testloader = t.utils.data.DataLoader(dropletData_test,
                                        batch_size=8,
                                        shuffle=False,\
                                            
                                        num_workers=1)
    classes = ('background', 'empty', 'multiple', 'single')

    # img = dropletData_test[0]

    #加载模型参数
    device = t.device("cuda:0" if t.cuda.is_available() else "cpu")

    drop_model = Net()
    drop_model.load_state_dict(t.load('./drop_net-1.0.pt',map_location = device))
    drop_model.eval()

    
    # device = t.device("cpu")

    # drop_model.to(device)

    with t.no_grad():
        _predictions = []
        for data in testloader:
            outputs = drop_model(data)
            scores,predicted = t.max(outputs,1)

            np_scores = scores.numpy()
            np_pres = predicted.numpy()

            for i in range(len(np_pres)):
                if np_pres[i] >= 2 and np_scores[i] < 0:
                    np_pres[i] = 1

            _predictions = np.append(_predictions,np_pres)
        
    
    # 保存结果
    _label_address = img_dir+'dropInformation\\'+img_name+'.txt'
    dropCircles = np.loadtxt(_label_address)
    with open(_label_address+'.txt','w') as circle_label:
        for j in range(0,len(_predictions)):
            prej = int(_predictions[j])-1
            if prej == -1:
                continue
            elif prej == 1:
                prej = 2
            elif prej == 2:
                prej = 1
            circle_label.write(f'{dropCircles[j,0]}\t{dropCircles[j,1]}\t{dropCircles[j,2]}\t{prej}\n')
        circle_label.close()