clc; clear all; close all;

P1Start=346; P1End=567; 
C1= 2; C2= 12;

data = '6 Pazdziernika';

filename = 'readings.csv';
readDataCSV = csvread(filename,P1Start,C1,[P1Start C1 P1End C2]);
readTableCSV = readtable(filename);
readTableCSV = readTableCSV(:,6);

time = table2array(readTableCSV);
time = string(time(P1Start:P1End));
time = time.';

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

%% Wykresy
figure
plot(adti)
set(gca,'xticklabel',time)
title('Wysokosc'); ylabel('m'); xlabel(data);

figure
plot(tem)
set(gca,'xticklabel',time)
title('Temperatura'); ylabel('*C'); xlabel(data);

figure
plot(hum)
set(gca,'xticklabel',time)
title('Wilgotnosc'); ylabel('%'); xlabel(data);

figure
plot(press)
set(gca,'xticklabel',time)
title('Cisnienie'); ylabel('hPa'); xlabel(data);

figure
plot(mq7)
set(gca,'xticklabel',time)
title('MQ7 - CO'); ylabel('ppm'); xlabel(data);

figure
plot(mq7D)
set(gca,'xticklabel',time)
title('mq7D')

figure
plot(pm1_0)
set(gca,'xticklabel',time)
title('pm1.0'); ylabel('ug/m^3'); xlabel(data);

figure
plot(pm2_5);
set(gca,'xticklabel',time);
title('pm2.5'); ylabel('ug/m^3'); xlabel(data);

figure
plot(pm10);
set(gca,'xticklabel',time);
title('pm10'); ylabel('ug/m^3'); xlabel(data);

%% Mapa
figure('Color','white')
latlim = [40 55];
lonlim = [15 25];
worldmap('Poland') %'Poland'
land = shaperead('landareas', 'UseGeoCoords', true);
geoshow(land, 'FaceColor', [0.15 0.5 0.15])
rivers = shaperead('worldrivers', 'UseGeoCoords', true);
geoshow(rivers, 'Color', 'blue')
lakes = shaperead('worldlakes', 'UseGeoCoords', true);
geoshow(lakes, 'FaceColor', 'blue')
geoshow(latd, long,'Marker', '*', 'Color', 'red');
