<<<<<<< HEAD
%1 Konvertera till gråskala
%2 Gör en kopia och unvertera färger
%3 Lägg på blur på kopian
%4 Color-dodge-merga de två bilderna

clear all
figure 
RGB = imread('gubbe.jpg');
imshow(RGB)
figure
subplot(2,2,1)
SV = rgb2gray(RGB); %SV = svartvit
imshow(SV)
subplot(2,2,2)
SV1 = imcomplement(SV);
imshow(SV1)
subplot(2,2,3)
B = imgaussfilt(SV1, 5);
imshow(B)
subplot(2,2,4)

mode = 'ColorDodge';

res = blendMode(B, SV, mode, 1, 1);
imshow(res)

rescopy = res;
ritar = draw(rescopy);





=======
%1 Konvertera till gråskala
%2 Gör en kopia och unvertera färger
%3 Lägg på blur på kopian
%4 Color-dodge-merga de två bilderna

clear all
figure 
RGB = imread('linda1.jpg');
imshow(RGB)
figure
subplot(2,2,1)
SV = rgb2gray(RGB); %SV = svartvit
imshow(SV)
subplot(2,2,2)
SV1 = imcomplement(SV);
imshow(SV1)
subplot(2,2,3)
B = imgaussfilt(SV1, 25);
imshow(B)
subplot(2,2,4)

mode = 'ColorDodge';

res = blendMode(B, SV, mode, 1, 1);
imshow(res)

rescopy = res;
[vx, vy] = size(rescopy);

max = vx*vy;
[ritar,Z,N] = draw(rescopy);

fid=fopen('output.txt','w');
fprintf(fid,'Number Start Coordinate Stop Coordinate Color/Hardness \r\n');
for x = 1:N-1
    fprintf(fid, '%d %s %d %d %d %d %1.10f \r\n',x,':',Z{x});
end
fclose(fid);




>>>>>>> master
