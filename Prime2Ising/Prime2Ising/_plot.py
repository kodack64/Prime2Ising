
import matplotlib.pyplot as plt;
import numpy as np;

plt.subplot(121);
fp = open("_ca.txt");
darray = [];
for line in fp:
    datas = line.split(" ");
    darray.append(datas[1:3]);
ndarray = np.array(darray);

x = np.arange(len(darray));
for ind in range(len(darray[0])):
    y = ndarray[:,ind];
    plt.plot(x,y);


plt.subplot(122);
fp = open("_qa.txt");
darray = [];
for line in fp:
    datas = line.split(" ");
    darray.append(datas[1:3]);
ndarray = np.array(darray);

x = np.arange(len(darray));
for ind in range(len(darray[0])):
    y = ndarray[:,ind];
    plt.plot(x,y);

    
plt.show();
