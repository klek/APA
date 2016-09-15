%1 Konvertera till gr�skala
%2 G�r en kopia och unvertera f�rger
%3 L�gg p� blur p� kopian
%4 Color-dodge-merga de tv� bilderna

clear all
figure 
RGB = imread('linda.jpg');
imshow(RGB)
figure
subplot(2,2,1)
SV = rgb2gray(RGB); %SV = svartvit
imshow(SV)
subplot(2,2,2)
SV1 = imcomplement(SV);
imshow(SV1)
subplot(2,2,3)
B = imgaussfilt(SV1, 30);
imshow(B)
subplot(2,2,4)

mode = 'ColorDodge';

res = blendMode(B, SV, mode, 1, 1);
imshow(res)

figure 
imshow(res)