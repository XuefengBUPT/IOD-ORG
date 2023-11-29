function [ hsv_std ] = hsiStd_fast_c( b,  HSV )
    %HSV Summary of this function goes here
    %   Detailed explanation goes here
    
    area = double(b(:,3) .* b(:,4));
    b(:,3) = b(:,1) + b(:,3) - 1;
    b(:,4) = b(:,2) + b(:,4) - 1;
    b = int32(b);
    H = HSV(:,:,1);
    S = HSV(:,:,2);
    V = HSV(:,:,3);

    H2 = H.^2;
    S2 = S.^2;
    V2 = V.^2;

    hsv_std = mex_hsi_std(H,S,V,H2,S2,V2,b,area);

    hsv_std(:,1) = sqrt(hsv_std(:,1)./area);
    hsv_std(:,2) = sqrt(hsv_std(:,2)./area);
    hsv_std(:,3) = sqrt(hsv_std(:,3)./area);
    
    % [H_inte] = compute_integralMap(H);
    % [S_inte] = compute_integralMap(S);
    % [V_inte] = compute_integralMap(V);
    % [H2_inte] = compute_integralMap(H.^2);
    % [S2_inte] = compute_integralMap(S.^2);
    % [V2_inte] = compute_integralMap(V.^2);
    % for i=1:size(b,1)
    %     %  get pixels in region
    %     H_sum = ...
    %         H_inte(b(i,4) + 1       , b(i,3) + 1) + ...
    %         H_inte(b(i,2)           , b(i,1)) - ...
    %         H_inte(b(i,4) + 1       , b(i,1)) - ...
    %         H_inte(b(i,2)           , b(i,3) + 1);
    %     S_sum = S_inte(b(i,4)+1,b(i,3)+1) + ...
    %         S_inte(b(i,2),b(i,1)) - ...
    %         S_inte(b(i,4)+1,b(i,1)) - ...
    %         S_inte(b(i,2),b(i,3)+1);
    %     V_sum = V_inte(b(i,4)+1,b(i,3)+1) + ...
    %         V_inte(b(i,2),b(i,1)) - ...
    %         V_inte(b(i,4)+1,b(i,1)) - ...
    %         V_inte(b(i,2),b(i,3)+1);
    %     H2_sum = H2_inte(b(i,4)+1,b(i,3)+1) + ...
    %         H2_inte(b(i,2),b(i,1)) - ...
    %         H2_inte(b(i,4)+1,b(i,1)) - ...
    %         H2_inte(b(i,2),b(i,3)+1);
    %     S2_sum = S2_inte(b(i,4)+1,b(i,3)+1) + ...
    %         S2_inte(b(i,2),b(i,1)) - ...
    %         S2_inte(b(i,4)+1,b(i,1)) - ...
    %         S2_inte(b(i,2),b(i,3)+1);
    %     V2_sum = V2_inte(b(i,4)+1,b(i,3)+1) + ...
    %         V2_inte(b(i,2),b(i,1)) - ...
    %         V2_inte(b(i,4)+1,b(i,1)) - ...
    %         V2_inte(b(i,2),b(i,3)+1);
    %     h_var = (H2_sum - area(i) * ((H_sum/area(i)) ^ 2));
    %     s_var = (S2_sum - area(i) * ((S_sum/area(i)) ^ 2));
    %     v_var = (V2_sum - area(i) * ((V_sum/area(i)) ^ 2));
    %     hsv_std(i,:) = [h_var,s_var,v_var];
    % end
    % hsv_std(:,1) = sqrt(hsv_std(:,1)./area);
    % hsv_std(:,2) = sqrt(hsv_std(:,2)./area);
    % hsv_std(:,3) = sqrt(hsv_std(:,3)./area);
    
    end
    
    