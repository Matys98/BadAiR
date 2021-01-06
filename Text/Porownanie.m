clc; clear all; close all;

P1Start=42-1; P1End=231; 
C1= 2; C2= 13;
step = 15;

data = '22 Grudnia 2020';

filename = 'readings (2).csv';
readDataCSV = csvread(filename,P1Start,C1,[P1Start C1 P1End C2]);
readTableCSV = readtable(filename);
readTableCSV = readTableCSV(:,6);

time = table2array(readTableCSV);
time = string(time(P1Start:step:P1End));
time = time.';
xTime = P1Start:P1End;

%% Oczyt danych
latd = readDataCSV(:,1);
long = readDataCSV(:,2);
adti = readDataCSV(:,3);
tem = readDataCSV(:,4);
hum = readDataCSV(:,5);
press = readDataCSV(:,6);
mq7 = readDataCSV(:,7);
mq7D = readDataCSV(:,8);
pm1_0 = readDataCSV(:,9);
pm2_5 = readDataCSV(:,10);
pm10 = readDataCSV(:,11);
BMP_Tem = readDataCSV(:,12);

%% Wykresy
figure
plot(xTime, tem)
hold on;
plot(xTime, BMP_Tem)
set(gca, 'XTick', P1Start:step:P1End, 'XTickLabel', time);
title('Porównanie odczytanych temperatur'); ylabel('*C'); xlabel(data);
legend("DHT22","BMP180")