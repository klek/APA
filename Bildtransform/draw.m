function [rita,Z,N] = draw(X)


n = 1;
[sx, sy] = size(X);

% A4 210 x 297

kx = 200; % storlek sx är samma som orginal
ky = 280; % storlek sy är samma som orginal
Mx = scaling(X, kx, 1);
M = scaling(Mx, ky, 2);
    
    
max = kx*ky;

Z = cell(max,1);
A = 0;
h = 0;
g = 0;
temp = 0;
figure
axis([0 kx 0 ky])

minstsk = 1;

while A < 1
    [A,I] = min(M(:));
        [h,g] = ind2sub([kx,ky],I);
        h1 = bildram(h);
        g1 = bildram(g);
        for j = 1:3
            for k = 1:3
                if(h1 == h) && (g1 == g)
                    
                else
                    temp = M(h1, g1);
                    skillnad = temp - A;
                    if(skillnad < minstsk)
                        minstsk = skillnad;
                        h2 = h1;
                        g2 = g1;
                    end

                end
                g1 = jamf(ky,g1);
            end
            g1 = bildram(g);   
            h1 = jamf(kx, h1);
        end
        h1 = bildram(h);
        Z{n} = [h g h2 g2 M(I)];
        if(M(I) < 1)
            line([h h2],[g g2],[1 1],'LineStyle','-','LineWidth',0.2,'Color', [0 0 0 1-M(I)])
        end
        
    M(I) = 1;
    minstsk = 1;
    n = n+1;
end
N = n;
%C = X(I);

rita = M;

end