function [rita,Z, E] = draw(X)


n = 1;
[sx, sy] = size(X);

max = sx*sy;
T = 1;
R = 1;
Z = cell(max,1);
E = cell(max,1);
A = 0;
h = 0;
g = 0;
figure
axis([0 sy 0 sx])

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
        Z{n} = [h2 g2];
        E{n} = [h g];
        line([g g2],[h h2],[1 1],'LineStyle','-','LineWidth',0.5,'Color', [0.5 0.5 0.5 X(I)])
%         if(n < max*0.1)
%             line([h h2],[g g2],[1 1],'LineStyle','-','LineWidth',0.5,'Color', [0 0 0 X(I)])
%         elseif(n > max*0.1 && n < max*0.2)
%             line([h h2],[g g2],[1 1],'LineStyle','-','LineWidth',0.45,'Color', [0 0 0 X(I)])
%         elseif(n > max*0.2 && max*0.3 > n)
%             line([h h2],[g g2],[1 1],'LineStyle','-','LineWidth',0.4,'Color', [0 0 0 X(I)])
%         elseif(n > max*0.3 && max*0.4 > n)
%             line([h h2],[g g2],[1 1],'LineStyle','-','LineWidth',0.35,'Color', [0 0 0 X(I)])
%         elseif(n > max*0.4 && max*0.5 > n)
%             line([h h2],[g g2],[1 1],'LineStyle','-','LineWidth',0.3,'Color', [0 0 0 X(I)])
%         elseif(n > max*0.5 && max*0.6 > n)
%             line([h h2],[g g2],[1 1],'LineStyle','-','LineWidth',0.25,'Color', [0 0 0 X(I)])
%         elseif(n > max*0.6 && max*0.7 > n)
%             line([h h2],[g g2],[1 1],'LineStyle','-','LineWidth',0.2,'Color', [0 0 0 X(I)])
%         elseif(n > max*0.7 && max*0.8 > n)
%             line([h h2],[g g2],[1 1],'LineStyle','-','LineWidth',0.15,'Color', [0 0 0 X(I)])
%         elseif(n > max*0.8 && max*0.9 > n)
%             line([h h2],[g g2],[1 1],'LineStyle','-','LineWidth',0.1,'Color', [0 0 0 X(I)])
%         elseif(n > max*0.9 && max > n)
%             line([h h2],[g g2],[1 1],'LineStyle','-','LineWidth',0.05,'Color', [0 0 0 X(I)])
%         end
    X(I) = 1;
    minstsk = 1;
    n = n+1
end

%C = X(I);

rita = X;

end