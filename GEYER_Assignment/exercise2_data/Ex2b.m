%% AML-Reflex Model Project
% Authors:
% Gaia Carparelli
% Marion Claudet
clc;
clear all;
close all;

%% Loading structures
nbr_cond = 8;

% LOADING HAM MUSCLES with 4 conditions
a = load('struct_mRFX_HF_asymetric_feedback.mat');
struct.HAM.asymetric_f = a.struct_mRFX_HF_asymetric_feedback;
b = load('struct_mRFX_HF_asymetric_stimulation.mat');
struct.HAM.asymetric_s = b.struct_mRFX_HF_asymetric_stimulation;
c = load('struct_mRFX_HF_symetric_feedback.mat');
struct.HAM.symetric_f = c.struct_mRFX_HF_symetric_feedback;
d = load('struct_mRFX_HF_symetric_stimulation.mat');
struct.HAM.symetric_s = d.struct_mRFX_HF_symetric_stimulation;

% LOADING TA MUSCLES with 4 conditions
f = load('struct_mRFX_TA_asymetric_feedback.mat');
struct.TA.asymetric_f = f.struct_mRFX_TA_asymetric;
g = load('struct_mRFX_TA_asymetric_stim.mat');
struct.TA.asymetric_s = g.struct_mRFX_TA_asymetric_stim;
h = load('struct_mRFX_TA_symetric_feedback.mat');
struct.TA.symetric_f = h.struct_mRFX_TA_symetric;
j = load('struct_mRFX_TA_symetric_stim.mat');
struct.TA.symetric_s = j.struct_mRFX_TA_symetric_stim;


%% Step length, Step duration + average speed

muscles = {'HAM','TA'};
conditions = {'symetric_f','symetric_s','asymetric_f','asymetric_s'};

for muscle = 1: length(muscles)
    for condition = 1: length(conditions)
        indices_stance_left.(muscles{muscle}).(conditions{condition}) = find(struct.(muscles{muscle}).(conditions{condition}).LStance == 1);
        indices_stance_right.(muscles{muscle}).(conditions{condition}) = find(struct.(muscles{muscle}).(conditions{condition}).RStance == 1);
    end
end

% For the left foot
for muscle = 1: length(muscles)
    for condition = 1: length(conditions)
        HS_left.(muscles{muscle}).(conditions{condition}) = [];
        for i = 1:length(indices_stance_left.(muscles{muscle}).(conditions{condition}))-1
            if (indices_stance_left.(muscles{muscle}).(conditions{condition})(i+1) - indices_stance_left.(muscles{muscle}).(conditions{condition})(i) > 1)
                HS_left.(muscles{muscle}).(conditions{condition}) = [HS_left.(muscles{muscle}).(conditions{condition}), indices_stance_left.(muscles{muscle}).(conditions{condition})(i+1)];
            end
        end
    end
end

% For the right foot
for muscle = 1: length(muscles)
    for condition = 1: length(conditions)
        HS_right.(muscles{muscle}).(conditions{condition}) = [];
        for i = 1:length(indices_stance_right.(muscles{muscle}).(conditions{condition}))-1
            if (indices_stance_right.(muscles{muscle}).(conditions{condition})(i+1) - indices_stance_right.(muscles{muscle}).(conditions{condition})(i) > 1)
                HS_right.(muscles{muscle}).(conditions{condition}) = [HS_right.(muscles{muscle}).(conditions{condition}), indices_stance_right.(muscles{muscle}).(conditions{condition})(i+1)];
            end
        end
    end
end

%% stride freq

nbr_cond = 8;

% Left foot
for muscle = 1:length(muscles)
    for condition = 1: length(conditions)
        stride_freq_total_left.(muscles{muscle}).(conditions{condition}) = [];
        for nbr=1:nbr_cond
            current_HS =  HS_left.(muscles{muscle}).(conditions{condition});
            for i = 1: length(current_HS) -1
                Results.left.(muscles{muscle}).(conditions{condition}){nbr}(i) = 1./ (current_HS(i+1) - current_HS(i));
            end
            stride_freq_total_left.(muscles{muscle}).(conditions{condition}) = [stride_freq_total_left.(muscles{muscle}).(conditions{condition}) , Results.left.(muscles{muscle}).(conditions{condition}){nbr}(i)];
        end
        
    end
end
for muscle = 1:length(muscles)
    for condition = 1: length(conditions)
        
        Results.left.mean_stride_freq.(muscles{muscle}).(conditions{condition}) = mean(stride_freq_total_left.(muscles{muscle}).(conditions{condition}));
    end
end

% Right foot
for muscle = 1:length(muscles)
    for condition = 1: length(conditions)
        stride_freq_total_right.(muscles{muscle}).(conditions{condition}) = [];
        for nbr=1:nbr_cond
            current_HS =  HS_right.(muscles{muscle}).(conditions{condition});
            for i = 1: length(current_HS) -1
                Results.right.(muscles{muscle}).(conditions{condition}){nbr}(i) = 1./ (current_HS(i+1) - current_HS(i));
            end
            stride_freq_total_right.(muscles{muscle}).(conditions{condition}) = [stride_freq_total_right.(muscles{muscle}).(conditions{condition}) , Results.right.(muscles{muscle}).(conditions{condition}){nbr}(i)];
        end
        
    end
end
for muscle = 1:length(muscles)
    for condition = 1: length(conditions)
        
        Results.right.mean_stride_freq.(muscles{muscle}).(conditions{condition}) = mean(stride_freq_total_left.(muscles{muscle}).(conditions{condition}));
    end
end
%%
% Left
sample_freq = 100; % 100 Hz
% empirical values
alpha = 0.85;
beta = 0.5;
for muscle = 1: length(muscles)
    for condition = 1: length(conditions)
        for nbr=1:nbr_cond
            current_mean_freq_stride =  Results.left.mean_stride_freq.(muscles{muscle}).(conditions{condition});
            Results.left.walking_speed_left.(muscles{muscle}).(conditions{condition}) =  ((current_mean_freq_stride * sample_freq)./alpha).^(1/beta);
        end
    end
end

% Right
sample_freq = 100; % 100 Hz
% empirical values
alpha = 0.85;
beta = 0.5;
for muscle = 1: length(muscles)
    for condition = 1: length(conditions)
        for nbr=1:nbr_cond
            current_mean_freq_stride =  Results.right.mean_stride_freq.(muscles{muscle}).(conditions{condition});
            Results.right.walking_speed_right.(muscles{muscle}).(conditions{condition}) =  ((current_mean_freq_stride * sample_freq)./alpha).^(1/beta);
        end
    end
end
%% Stride length

% Left
for muscle = 1: length(muscles)
    for condition = 1: length(conditions)
        for nbr=1:nbr_cond
            current_mean_duration_stride =  1./Results.left.mean_stride_freq.(muscles{muscle}).(conditions{condition});
            current_mean_speed_stride = Results.left.walking_speed_left.(muscles{muscle}).(conditions{condition});
            Results.left.mean_length_stride.(muscles{muscle}).(conditions{condition}) = current_mean_speed_stride * current_mean_duration_stride;
        end
    end
end

% Right
for muscle = 1: length(muscles)
    for condition = 1: length(conditions)
        for nbr=1:nbr_cond
            current_mean_duration_stride =  1./Results.right.mean_stride_freq.(muscles{muscle}).(conditions{condition});
            current_mean_speed_stride = Results.right.walking_speed_right.(muscles{muscle}).(conditions{condition});
            Results.right.mean_length_stride.(muscles{muscle}).(conditions{condition}) = current_mean_speed_stride * current_mean_duration_stride;
        end
    end
end