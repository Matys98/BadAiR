clc; clear all; close all;

R1=100; C1= 2;
R2=560; C2= 12;

filename = 'readings.csv';
readDataCSV = csvread(filename,R1,C1,[R1 C1 R2 C2]);
readTableCSV = readtable(filename);
readTableCSV = readTableCSV(:,6);

time = table2array(readTableCSV);
time = string(time)

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
set(gca,'xticklabel',time(R1:R2).')
title('adti')
figure
plot(tem)
set(gca,'xticklabel',time(R1:R2).')
title('tem')
figure
plot(hum)
set(gca,'xticklabel',time(R1:R2).')
title('hum')
figure
plot(press)
set(gca,'xticklabel',time(R1:R2).')
title('press')
figure
plot(mq7)
set(gca,'xticklabel',time(R1:R2).')
title('mq7')
figure
plot(mq7D)
set(gca,'xticklabel',time(R1:R2).')
title('mq7D')
figure
plot(pm1_0)
set(gca,'xticklabel',time(R1:R2).')
title('pm1.0')
figure
plot(pm2_5)
set(gca,'xticklabel',time(R1:R2).')
title('pm2.5')
figure
plot(pm10)
set(gca,'xticklabel',time(R1:R2).')
title('pm10')

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
geoshow(latd, long,'Marker', '.', 'Color', 'red');
