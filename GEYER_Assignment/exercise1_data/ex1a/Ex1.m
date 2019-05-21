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
    
    FF.indices_right{nbr} = find(FF.(foot_falls{nbr})(:,1) == 1);
    
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
    % Stride duration mean and std
    FF.stride_mean_duration_left{nbr} = mean(FF.(strides_durations_left{nbr}));
    FF.stride_std_duration_left{nbr} = std(FF.(strides_durations_left{nbr}));
    
    % Stride frequency mean and std
    FF.stride_mean_freq_left{nbr} = mean(FF.(strides_durations_left{nbr}));
    FF.stride_std_freq_left{nbr} = std(FF.(strides_durations_left{nbr}));
    
    % Stance duration mean and std
    FF.stance_mean_duration_left{nbr} = mean(FF.(stances_durations_left{nbr}));
    FF.stance_std_duration_left{nbr} = std(FF.(stances_durations_left{nbr}));
    
    % Swing duration mean and std
    FF.swing_mean_duration_left{nbr} = mean(FF.(swings_durations_left{nbr}));
    FF.swing_std_duration_left{nbr} = std(FF.(swings_durations_left{nbr}));
    
    
    
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
    % Stride duration mean and std
    FF.stride_mean_duration_right{nbr} = mean(FF.(strides_durations_right{nbr}));
    FF.stride_std_duration_right{nbr} = std(FF.(strides_durations_right{nbr}));
    
    % Stride frequency mean and std
    FF.stride_mean_freq_right{nbr} = mean(FF.(strides_durations_right{nbr}));
    FF.stride_std_freq_right{nbr} = std(FF.(strides_durations_right{nbr}));
    
    % Stance duration mean and std
    FF.stance_mean_duration_right{nbr} = mean(FF.(stances_durations_right{nbr}));
    FF.stance_std_duration_right{nbr} = std(FF.(stances_durations_right{nbr}));
    
    % Swing duration mean and std
    FF.swing_mean_duration_right{nbr} = mean(FF.(swings_durations_right{nbr}));
    FF.swing_std_duration_right{nbr} = std(FF.(swings_durations_right{nbr}));
    
    
end

%% Swing and double stance durations



