clear;
clc;
% 
drop_information=[];
for i = 28:1:51
    name = ['drop_information_for_' ,num2str(i),'.txt'];
    drop_information1 = load(name);
    drop_information=cat(1,drop_information,drop_information1);
end
times = 20;
drop_radius = 2*drop_information(:,3)*13.7/times;
min_radius = min(drop_radius)
max_radius = max(drop_radius)
mean_radius = mean(drop_radius)
std_radius = std(drop_radius)
CV=100*std_radius/mean_radius
median_radius = median(drop_radius)
a=hist(drop_radius,10);
hist(drop_radius,30);