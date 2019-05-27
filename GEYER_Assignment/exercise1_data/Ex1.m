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
FF.stride_duration_left_total = [];
FF.stance_duration_left_total = [];
FF.swing_duration_left_total = [];
FF.strides_freqs_left_total = [];
for nbr=1:nbr_data
    current_HS = FF.HS_left{nbr};
    current_TO = FF.TO_left{nbr};
    
    for i = 1: length(current_HS) -1
        % stride
        FF.(strides_durations_left{nbr})(i) = current_HS(i+1) - current_HS(i);
        FF.(strides_freqs_left{nbr})(i) = 1./ (current_HS(i+1) - current_HS(i)); % stride freq does not have the factor 2!!!
        
        % stance
        FF.(stances_durations_left{nbr})(i) = current_TO(i+1) - current_HS(i);
        
        % swing
        FF.(swings_durations_left{nbr})(i) = current_HS(i) - current_TO(i);
    end
    FF.stride_duration_left_total = [FF.stride_duration_left_total, FF.(strides_durations_left{nbr})];
    FF.stance_duration_left_total = [FF.stance_duration_left_total, FF.(stances_durations_left{nbr})];
    FF.swing_duration_left_total = [FF.swing_duration_left_total, FF.(swings_durations_left{nbr})];
    FF.strides_freqs_left_total = [FF.strides_freqs_left_total, FF.(strides_freqs_left{nbr})];
end


% Right foot
FF.stride_duration_right_total = [];
FF.stance_duration_right_total = [];
FF.swing_duration_right_total = [];
FF.strides_freqs_right_total = [];
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
    FF.stride_duration_right_total = [FF.stride_duration_right_total, FF.(strides_durations_right{nbr})];
    FF.stance_duration_right_total = [FF.stance_duration_right_total, FF.(stances_durations_right{nbr})];
    FF.swing_duration_right_total = [FF.swing_duration_right_total, FF.(swings_durations_right{nbr})];
    FF.strides_freqs_right_total = [FF.strides_freqs_right_total, FF.(strides_freqs_right{nbr})];
end

% Both feet
FF.stride_duration_total = [FF.stride_duration_left_total, FF.stride_duration_right_total];
FF.stance_duration_total = [FF.stance_duration_left_total, FF.stance_duration_right_total];
FF.swing_duration_total = [FF.swing_duration_left_total, FF.swing_duration_right_total];
FF.strides_freqs_total = [FF.strides_freqs_left_total,FF.strides_freqs_right_total];

Final_values.mean_stride_duration = mean(FF.stride_duration_total);
Final_values.mean_stance_duration = mean(FF.stance_duration_total);
Final_values.mean_swing_duration = mean(FF.swing_duration_total);
Final_values.mean_strides_freqs = mean(FF.strides_freqs_total);

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
FF.double_stance_duration_total = [];
for nbr=1:nbr_data
    current_foot_flat_start = FF.foot_flat_start{nbr};
    current_foot_flat_end = FF.foot_flat_end{nbr};
    
    for i = 1: length(current_foot_flat_start)/2 -1
        FF.(double_stance_durations{nbr})(i) = ((current_foot_flat_end(i) - current_foot_flat_start(i)) + (current_foot_flat_end(i+1) - current_foot_flat_start(i+1))) ;      
    end
    FF.double_stance_duration_total = [FF.double_stance_duration_total, FF.(double_stance_durations{nbr})];
end
FF.double_stance_duration_total = [FF.double_stance_duration_total, FF.double_stance_duration_total]; %vector is doubled since it contains double stance durations for left and right feet
Final_values.mean_double_stance_duration = mean(FF.double_stance_duration_total);


%% Calculate the average proportions

Result.mean_stride_over_stance = Final_values.mean_stride_duration/Final_values.mean_stance_duration;
Result.mean_stance_over_swing = Final_values.mean_stance_duration/Final_values.mean_swing_duration;
Result.mean_swing_over_double_stance = Final_values.mean_swing_duration/Final_values.mean_double_stance_duration;

Result.std_stride_over_stance = std(FF.stride_duration_total./FF.stance_duration_total);
Result.std_stance_over_swing = std(FF.stance_duration_total./FF.swing_duration_total);

Min = min(length(FF.swing_duration_total),length(FF.double_stance_duration_total));
Result.std_swing_over_double_stance = std(FF.swing_duration_total(1:Min)./FF.double_stance_duration_total(1:Min));

errorbar([Result.mean_stride_over_stance,Result.mean_stance_over_swing,Result.mean_swing_over_double_stance],[Result.std_stride_over_stance,Result.std_stance_over_swing,Result.std_swing_over_double_stance],'ko','LineWidth',1.2);
hold on

plot(linspace(0,4,20), ((1+sqrt(5))/2)*ones(1,20), '--r','LineWidth',1.2);
text(0,(1+sqrt(5))/2,'$\phi = \frac{1 + \sqrt{5}}{2}$','HorizontalAlignment','right','FontSize',40,'interpreter','latex')
% ['PHI =' num2str((1+sqrt(5)/2))]
axis([0, 4, 1, 2]);
ax = gca;
ax.FontSize = 40;

set(gca,'TickLabelInterpreter', 'latex','XTick',([1 2 3]),'XTickLabel',{'$\frac{Stride}{Stance}$','$\frac{Stance}{Swing}$','$\frac{Swing}{Double \ stance}$'});
set(gca,'TickLabelInterpreter', 'latex', 'YTick',[1,1.2,1.4,1.8,2]);
title('Proportions among gait phases','interpreter','latex')
set(gcf,'Position',[0 0 1600 800]);
saveas(gcf,'golden_ratio','png');


%% Evaluate the walking speed
sample_freq = 100; % 100 Hz
% empirical values
alpha = 0.85; 
beta = 0.5;
for nbr=1:nbr_data
    walking_speed_left{nbr} =  ((mean(FF.(strides_freqs_left{nbr}))* sample_freq)./alpha).^(1/beta);
    walking_speed_right{nbr} =  ((mean(FF.(strides_freqs_right{nbr}))* sample_freq)./alpha).^(1/beta);
end
for nbr=1:nbr_data
Final_values.walking_speed{nbr} = (walking_speed_left{nbr} + walking_speed_right{nbr})/2; % mean between left and right foot
end
%% EXERCISE 1b

%% Ground reaction forces - comparing running and walking
data_to_compare1 = load('data1b.mat');
%data_to_compare2 = load('data2b.mat');
% Plotting GRF

% First data mat
GRF1 = data_to_compare1.data1.grf.data;
GRF2 = data_to_compare1.data2.grf.data;
GRF3 = data_to_compare1.data3.grf.data;



figure(1);
freq = 100; % 100 Hz, sampling rate
time_indices_1 = 1:1:length(GRF1(:,2));
time_points_1 = time_indices_1/freq;
time_indices_2 = 1:1:length(GRF2(:,2));
time_points_2 = time_indices_2/freq;

XMIN = 0;
XMAX = 5;
YMIN = 0;
YMAX = 3200;

plot((time_indices_1/freq),GRF1(:,2),'r','LineWidth',1.4);
hold on;
plot((time_indices_2/freq),GRF2(:,2),'b','LineWidth',1.4);

legend('walking','running','interpreter','latex');
ax = gca;
ax.FontSize = 40;
axis([XMIN XMAX YMIN YMAX])
set(gca,'TickLabelInterpreter', 'latex')
set(gcf,'Position',[0 0 1600 800]);
xlabel('time [s]','interpreter','latex')
ylabel('GRF','interpreter','latex')
title('Comparison between running and walking','interpreter','latex')
saveas(gcf,'running_walking','png');

FFb.foot_falls1 = data_to_compare1.data1.footfall.data;
FFb.foot_falls2 = data_to_compare1.data2.footfall.data;
FFb.foot_falls3 = data_to_compare1.data3.footfall.data;
foot_falls = {'foot_falls1','foot_falls2','foot_falls3'};

nbr_data = 3;
for nbr=1:nbr_data
    FFb.indices_both{nbr} = find(FFb.(foot_falls{nbr})(:,1) == 1 & FFb.(foot_falls{nbr})(:,2) == 1);
end

