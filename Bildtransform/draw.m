function rita = draw(X)

n = 1;
[sx, sy] = size(X);
figure 
T = 1;

A = 0;
minstsk = 1;
while A < 1
    [A,I] = min(X(:));
        [h,g] = ind2sub([sx,sy],I);
        h1 = bildram(h);
        g1 = bildram(g);
        for j = 1:3
            for k = 1:3
                if(h1 == h) && (g1 == g)

                else
                    temp = X(h1, g1);
                    skillnad = temp - A;
                    if(skillnad < minstsk)
                        minstsk = skillnad;
                        h2 = h1;
                        g2 = g1;

                    end

                end
                if(g1 >= sy)
                    g1 = sy;
                elseif(g1 <= 1) 
                    g1 = 1;
                else
                    g1 = g1+1;
                end
            end
            g1 = bildram(g);   
            if(h1 >= sx)
                h1 = sx;
            elseif(h1 <= 1) 
                h1 = 1;
            else
                h1 = h1+1;
            end
        end
        h1 = bildram(h);
        if(T > 10)
             line([h g], [h2 g2],[1 1],'Marker','.','LineStyle','-')
             T = 1;
        end
    T = T+1;
    X(I) = 1;
    n = n+1
end


axis([0 sx 0 sy])

%C = X(I);

rita = X;

end