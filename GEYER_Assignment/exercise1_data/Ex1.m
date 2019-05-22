%% AML-Reflex Model Project
% Authors:
% Gaia Carparelli
% Marion Claudet
clc;
clear all;
close all;
%% Load data
load('data1.mat');
load('data2.mat');
load('data3.mat');
%% Extracting footfall patterns

nbr_data = 3;

FF.foot_falls1 = data1.footfall.data;
FF.foot_falls2 = data2.footfall.data;
FF.foot_falls3 = data3.footfall.data;


% Left foot
foot_falls = {'foot_falls1','foot_falls2','foot_falls3'};
FF.HS_left{1} = [];
FF.HS_left{2} = [];
FF.HS_left{3} = [];

FF.TO_left{1} = [];
FF.TO_left{2} = [];
FF.TO_left{3} = [];

for nbr=1:nbr_data
    
    FF.indices_left{nbr} = find(FF.(foot_falls{nbr})(:,1) == 1);
    
    for i = 1:length(FF.indices_left{nbr})-1
        
        if (FF.indices_left{nbr}(i+1) - FF.indices_left{nbr}(i) > 1)
            FF.HS_left{nbr} = [FF.HS_left{nbr}, FF.indices_left{nbr}(i+1)];
            FF.TO_left{nbr} = [FF.TO_left{nbr}, FF.indices_left{nbr}(i)];
            
        end
    end
end
% Right foot
foot_falls = {'foot_falls1','foot_falls2','foot_falls3'};
FF.HS_right{1} = [];
FF.HS_right{2} = [];
FF.HS_right{3} = [];

FF.TO_right{1} = [];
FF.TO_right{2} = [];
FF.TO_right{3} = [];

for nbr=1:nbr_data
    
    FF.indices_right{nbr} = find(FF.(foot_falls{nbr})(:,2) == 1);
    
    for i = 1:length(FF.indices_right{nbr})-1
        
        if (FF.indices_right{nbr}(i+1) - FF.indices_right{nbr}(i) > 1)
            FF.HS_right{nbr} = [FF.HS_right{nbr}, FF.indices_right{nbr}(i+1)];
            FF.TO_right{nbr} = [FF.TO_right{nbr}, FF.indices_right{nbr}(i)];
            
        end
    end
end
%% Stride frequency (mean, std in Hz)
% Left foot
strides_freqs_left = {'stride_freq_left1','stride_freq_left2','stride_freq_left3'};
strides_durations_left = {'stride_duration_left1','stride_duration_left2','stride_duration_left3'};
stances_durations_left = {'stance_duration_left1','stance_duration_left2','stance_duration_left3'};
swings_durations_left = {'swing_duration_left1','swing_duration_left2','swing_duration_left3'};

% Right foot
strides_freqs_right = {'stride_freq_right1','stride_freq_right2','stride_freq_right3'};
strides_durations_right = {'stride_duration_right1','stride_duration_right2','stride_duration_right3'};
stances_durations_right = {'stance_duration_right1','stance_duration_right2','stance_duration_right3'};
swings_durations_right = {'swing_duration_right1','swing_duration_right2','swing_duration_right3'};

% Left foot
for nbr=1:nbr_data
    current_HS = FF.HS_left{nbr};
    current_TO = FF.TO_left{nbr};
    
    for i = 1: length(current_HS) -1
        % stride
        FF.(strides_durations_left{nbr})(i) = current_HS(i+1) - current_HS(i);
        FF.(strides_freqs_left{nbr})(i) = 1./ (current_HS(i+1) - current_HS(i));
        
        % stance
        FF.(stances_durations_left{nbr})(i) = current_TO(i+1) - current_HS(i);
        
        % swing
        FF.(swings_durations_left{nbr})(i) = current_HS(i) - current_TO(i);
    end

    
end

% Right foot
for nbr=1:nbr_data
    current_HS = FF.HS_right{nbr};
    current_TO = FF.TO_right{nbr};
    
    for i = 1: length(current_HS) -1
        
        % stride
        FF.(strides_durations_right{nbr})(i) = current_HS(i+1) - current_HS(i);
        FF.(strides_freqs_right{nbr})(i) = 1./ (current_HS(i+1) - current_HS(i));
        
        % stance
        FF.(stances_durations_right{nbr})(i) = current_TO(i+1) - current_HS(i);
        
        % swing
        FF.(swings_durations_right{nbr})(i) = current_HS(i) - current_TO(i);
    end

    
end
%% Double stance

FF.foot_flat_start{1} = [];
FF.foot_flat_start{2} = [];
FF.foot_flat_start{3} = [];

FF.foot_flat_end{1} = [];
FF.foot_flat_end{2} = [];
FF.foot_flat_end{3} = [];

double_stance_durations= {'double_stance_duration1','double_stance_duration2','double_stance_duration3'};
for nbr=1:nbr_data
    
    FF.indices_both{nbr} = find(FF.(foot_falls{nbr})(:,1) == 1 & FF.(foot_falls{nbr})(:,2) == 1);
    FF.foot_flat_start{nbr}(1) = FF.indices_both{nbr}(1);
end

for nbr=1:nbr_data

    for i = 1:length(FF.indices_both{nbr})-1
        
        if (FF.indices_both{nbr}(i+1) - FF.indices_both{nbr}(i) > 1)
            FF.foot_flat_start{nbr} = [FF.foot_flat_start{nbr}, FF.indices_both{nbr}(i+1)];
            FF.foot_flat_end{nbr} = [FF.foot_flat_end{nbr}, FF.indices_both{nbr}(i)];
        end
    end
end 
for nbr=1:nbr_data
    current_foot_flat_start = FF.foot_flat_start{nbr};
    current_foot_flat_end = FF.foot_flat_end{nbr};
    
    for i = 1: length(current_foot_flat_start) -1
        
        FF.(double_stance_durations{nbr})(i) = current_foot_flat_end(i) - current_foot_flat_start(i);
        
    end
end
%% Calculate the average proportions


for nbr=1:nbr_data
    
% Gait cycle/ Stance
current_stride_over_stance_left = FF.(strides_durations_left{nbr})./FF.(stances_durations_left{nbr});
current_stride_over_stance_right = FF.(strides_durations_right{nbr})./FF.(stances_durations_right{nbr});
current_stride_over_stance = [current_stride_over_stance_left,current_stride_over_stance_right];
% Mean
Results.mean_stride_over_stance{nbr} = mean(current_stride_over_stance);
% Std
Results.std_stride_over_stance{nbr} = std(current_stride_over_stance);

% Stance/ Swing
current_stance_over_swing_left = FF.(stances_durations_left{nbr})./FF.(swings_durations_left{nbr});
current_stance_over_swing_right = FF.(stances_durations_right{nbr})./FF.(swings_durations_right{nbr});
current_stance_over_swing = [current_stance_over_swing_left,current_stance_over_swing_right];
% Mean
Results.mean_stance_over_swing{nbr} = mean(current_stance_over_swing);
% Std
Results.std_stance_over_swing{nbr} = std(current_stance_over_swing);

% % Swing / Double Stance
% current_swing_over_double_stance_left = FF.(swings_durations_left{nbr})./FF.(double_stance_durations{nbr});
% current_swing_over_double_stance_right = FF.(swings_durations_right{nbr})./FF.(double_stance_durations{nbr});
% current_swing_over_double_stance =[current_swing_over_double_stance_left,current_swing_over_double_stance_right];
% % Mean
% Results.mean_swing_over_double_stance{nbr} = mean(current_swing_over_double_stance);
% % Std
% Results.std_swing_over_double_stance{nbr} = std(current_swing_over_double_stance);
 end

%% Evaluate the walking speed
sample_freq = 100; % 100 Hz
% empirical values
alpha = 0.85; 
beta = 0.5;
for nbr=1:nbr_data
    
    walking_speed_left{nbr} =  ((mean(FF.(strides_freqs_left{nbr}))* sample_freq)./alpha).^(1/beta);
    walking_speed_right{nbr} =  ((mean(FF.(strides_freqs_right{nbr}))* sample_freq)./alpha).^(1/beta);
end
%% EXERCISE 1b
clc;
clear all;
close all
%% Ground reaction forces - comparing running and walking
data_to_compare1 = load('data1b.mat');
%data_to_compare2 = load('data2b.mat');
% Plotting GRF

% First data mat
GRF1 = data_to_compare1.data1.grf.data;
GRF2 = data_to_compare1.data2.grf.data;
GRF3 = data_to_compare1.data3.grf.data;


XMIN = 0;
XMAX = 300;
YMIN = 0;
YMAX = 3200;
figure(1);
%plot(first_GRF1(:,2));
plot(GRF2(:,2));
hold on;
plot(GRF3(:,2));
legend('2','3');
axis([XMIN XMAX YMIN YMAX])

FFb.foot_falls1 = data_to_compare1.data1.footfall.data;
FFb.foot_falls2 = data_to_compare1.data2.footfall.data;
FFb.foot_falls3 = data_to_compare1.data3.footfall.data;
foot_falls = {'foot_falls1','foot_falls2','foot_falls3'};

nbr_data = 3;
for nbr=1:nbr_data
    FFb.indices_both{nbr} = find(FFb.(foot_falls{nbr})(:,1) == 1 & FFb.(foot_falls{nbr})(:,2) == 1);
end

