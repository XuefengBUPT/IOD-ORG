function [I] = get_obsmap(img,bbox,top)
%GET_OBSMAP �˴���ʾ�йش˺�����ժҪ
%   �˴���ʾ��ϸ˵��
I = img;
count = top;%size(bbox,1)
w = bbox(:,5);
w(w > 1) = 1;

for i = 1:count
    I = insertShape(I,'FilledRectangle',bbox(i,1:4),'Color',{'red'},'Opacity', w(i)/2);
end
end

