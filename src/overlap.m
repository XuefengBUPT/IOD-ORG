function [iou] = overlap(A,B)
%OVERLAP �˴���ʾ�йش˺�����ժҪ
%   �˴���ʾ��ϸ˵��
C=[A;B];
C = unique(C);
D = intersect(A,B);

iou = numel(D)/numel(C);
end

