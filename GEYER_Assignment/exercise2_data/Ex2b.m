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
a = load('struct_mRFX_HAM_asymetric_feedback.mat');
struct.noise.HAM.asymetric_f = a.struct_mRFX_HAM_asymetric_feedback;
b = load('struct_mRFX_HAM_asymetric_stimulation.mat');
struct.noise.HAM.asymetric_s = b.struct_mRFX_HAM_asymetric_stimulation;
c = load('struct_mRFX_HAM_symetric_feedback.mat');
struct.noise.HAM.symetric_f = c.struct_mRFX_HAM_symetric_feedback;
d = load('struct_mRFX_HAM_symetric_stimulation.mat');
struct.noise.HAM.symetric_s = d.struct_mRFX_HAM_symetric_stimulation;

% LOADING TA MUSCLES with 4 conditions
f = load('struct_mRFX_TA_asymetric_feedback.mat');
struct.noise.TA.asymetric_f = f.struct_mRFX_TA_asymetric_feedback;
g = load('struct_mRFX_TA_asymetric_stimulation.mat');
struct.noise.TA.asymetric_s = g.struct_mRFX_TA_asymetric_stimulation;
h = load('struct_mRFX_TA_symetric_feedback.mat');
struct.noise.TA.symetric_f = h.struct_mRFX_TA_symetric_feedback;
j = load('struct_mRFX_TA_symetric_stimulation.mat');
struct.noise.TA.symetric_s = j.struct_mRFX_TA_symetric_stimulation;

% To check, also loading struct_mRFX no noise
z =  load('struct_mRFX');
struct.no_noise = z.struct_mRFX;

%% Step length, Step duration + average speed

muscles = {'HAM','TA'};
conditions = {'asymetric_f','asymetric_s','symetric_f','symetric_s'};
perturbation = {'noise','no_noise'};

% Noise
for muscle = 1: length(muscles)
    for condition = 1: length(conditions)
        indices_stance_left.noise.(muscles{muscle}).(conditions{condition}) = find(struct.noise.(muscles{muscle}).(conditions{condition}).LStance == 1);
        indices_stance_right.noise.(muscles{muscle}).(conditions{condition}) = find(struct.noise.(muscles{muscle}).(conditions{condition}).RStance == 1);
    end
end
% No noise
indices_stance_left.no_noise = find(struct.no_noise.LStance == 1);
indices_stance_right.no_noise = find(struct.no_noise.RStance == 1);

% Noise
% For the left foot
for muscle = 1: length(muscles)
    for condition = 1: length(conditions)
        HS_left.noise.(muscles{muscle}).(conditions{condition}) = [];
        for i = 1:length(indices_stance_left.noise.(muscles{muscle}).(conditions{condition}))-1
            if (indices_stance_left.noise.(muscles{muscle}).(conditions{condition})(i+1) - indices_stance_left.noise.(muscles{muscle}).(conditions{condition})(i) > 1)
                ind = indices_stance_left.noise.(muscles{muscle}).(conditions{condition})(i+1);
                if (i+101<=length(indices_stance_left.noise.(muscles{muscle}).(conditions{condition})))
                    test = struct.noise.(muscles{muscle}).(conditions{condition}).LStance(ind:ind+100);
                    if (~(ismember(0,test)))
                        HS_left.noise.(muscles{muscle}).(conditions{condition}) = [HS_left.noise.(muscles{muscle}).(conditions{condition}), indices_stance_left.noise.(muscles{muscle}).(conditions{condition})(i+1)];
                    end
                end
            end
        end
        %uncomment to visualize the HS detections
%         figure;
%         plot(struct.noise.(muscles{muscle}).(conditions{condition}).LStance);
%         title(strcat('LEFT: muscle ', num2str(muscle), ', condition ', num2str(condition)));
%         hold on
%         scatter(HS_left.(muscles{muscle}).(conditions{condition}), ones(1,length(HS_left.(muscles{muscle}).(conditions{condition}))));
    end
end

% No noise
HS_left.no_noise = [];
for i = 1:length(indices_stance_left.no_noise)-1
    if (indices_stance_left.no_noise(i+1) - indices_stance_left.no_noise(i) > 1)
        ind = indices_stance_left.no_noise(i+1);
        if (i+101<=length(indices_stance_left.no_noise))
            test = struct.no_noise.LStance(ind:ind+100);
            if (~(ismember(0,test)))
                HS_left.no_noise = [HS_left.no_noise, indices_stance_left.no_noise(i+1)];
            end
        end
    end
end

% For the right foot
for muscle = 1: length(muscles)
    for condition = 1: length(conditions)
        HS_right.noise.(muscles{muscle}).(conditions{condition}) = [];
        for i = 1:length(indices_stance_right.noise.(muscles{muscle}).(conditions{condition}))-1
            if (indices_stance_right.noise.(muscles{muscle}).(conditions{condition})(i+1) - indices_stance_right.noise.(muscles{muscle}).(conditions{condition})(i) > 1)
                ind = indices_stance_right.noise.(muscles{muscle}).(conditions{condition})(i+1);
                if (i+101<=length(indices_stance_right.noise.(muscles{muscle}).(conditions{condition})))
                    test = struct.noise.(muscles{muscle}).(conditions{condition}).RStance(ind:ind+100);
                    if (~(ismember(0,test)))
                        HS_right.noise.(muscles{muscle}).(conditions{condition}) = [HS_right.noise.(muscles{muscle}).(conditions{condition}), indices_stance_right.noise.(muscles{muscle}).(conditions{condition})(i+1)];
                    end
                end
            end
        end
        %uncomment to visualize the HS detections
%         figure;
%         plot(struct.noise.(muscles{muscle}).(conditions{condition}).LStance);
%         title(strcat('RIGHT: muscle ', num2str(muscle), ', condition ', num2str(condition)));
%         hold on
%         scatter(HS_left.(muscles{muscle}).(conditions{condition}), ones(1,length(HS_left.(muscles{muscle}).(conditions{condition}))));
    end
end

% No noise
HS_right.no_noise = [];
for i = 1:length(indices_stance_right.no_noise)-1
    if (indices_stance_right.no_noise(i+1) - indices_stance_right.no_noise(i) > 1)
        ind = indices_stance_right.no_noise(i+1);
        if (i+101<=length(indices_stance_right.no_noise))
            test = struct.no_noise.RStance(ind:ind+100);
            if (~(ismember(0,test)))
                HS_right.no_noise = [HS_right.no_noise, indices_stance_right.no_noise(i+1)];
            end
        end
    end
end

%% stride freq

nbr_cond = 8;
sample_freq = 100; % 100 Hz

% Left foot- noise
for muscle = 1:length(muscles)
    for condition = 1: length(conditions)
        stride_freq_total_left.noise.(muscles{muscle}).(conditions{condition}) = [];
        for nbr=1:nbr_cond
            current_HS =  HS_left.noise.(muscles{muscle}).(conditions{condition});
            for i = 1: length(current_HS) -1
                Results.left.noise.(muscles{muscle}).(conditions{condition}).stride_freq{nbr}(i) = 2./ (current_HS(i+1) - current_HS(i));
            end
            stride_freq_total_left.noise.(muscles{muscle}).(conditions{condition}) = [stride_freq_total_left.noise.(muscles{muscle}).(conditions{condition}) , Results.left.noise.(muscles{muscle}).(conditions{condition}).stride_freq{nbr}(i)];
        end
    end
end
for muscle = 1:length(muscles)
    for condition = 1: length(conditions)
        Results.left.noise.mean_stride_freq.(muscles{muscle}).(conditions{condition}) = sample_freq *  mean((stride_freq_total_left.noise.(muscles{muscle}).(conditions{condition})));
    end
end


% no noise
stride_freq_total_left.no_noise = [];
for nbr=1:nbr_cond
    current_HS =  HS_left.no_noise;
    for i = 1: length(current_HS) -1
        Results.left.no_noise.stride_freq{nbr}(i) = 2./ (current_HS(i+1) - current_HS(i));
    end
    stride_freq_total_left.no_noise = [stride_freq_total_left.no_noise , Results.left.no_noise.stride_freq{nbr}(i)];
end
Results.left.no_noise.mean_stride_freq = sample_freq *  mean((stride_freq_total_left.no_noise));

% Right foot
for muscle = 1:length(muscles)
    for condition = 1: length(conditions)
        stride_freq_total_right.noise.(muscles{muscle}).(conditions{condition}) = [];
        for nbr=1:nbr_cond
            current_HS =  HS_right.noise.(muscles{muscle}).(conditions{condition});
            for i = 1: length(current_HS) -1
                Results.right.noise.(muscles{muscle}).(conditions{condition}).stride_freq{nbr}(i) = 2./ (current_HS(i+1) - current_HS(i));
            end
            stride_freq_total_right.noise.(muscles{muscle}).(conditions{condition}) = [stride_freq_total_right.noise.(muscles{muscle}).(conditions{condition}) , Results.right.noise.(muscles{muscle}).(conditions{condition}).stride_freq{nbr}(i)];
        end
        
    end
end
for muscle = 1:length(muscles)
    for condition = 1: length(conditions)
        Results.right.noise.mean_stride_freq.(muscles{muscle}).(conditions{condition}) = sample_freq*mean((stride_freq_total_right.noise.(muscles{muscle}).(conditions{condition})));
    end
end


% no noise
stride_freq_total_right.no_noise = [];
for nbr=1:nbr_cond
    current_HS =  HS_right.no_noise;
    for i = 1: length(current_HS) -1
        Results.right.no_noise.stride_freq{nbr}(i) = 2./ (current_HS(i+1) - current_HS(i));
    end
    stride_freq_total_right.no_noise = [stride_freq_total_right.no_noise , Results.right.no_noise.stride_freq{nbr}(i)];
end
Results.right.no_noise.mean_stride_freq = sample_freq *  mean((stride_freq_total_right.no_noise));


%%
% Left- noise
% empirical values
alpha = 0.85;
beta = 0.5;
for muscle = 1: length(muscles)
    for condition = 1: length(conditions)
        for nbr=1:nbr_cond
            current_mean_freq_stride =  Results.left.noise.mean_stride_freq.(muscles{muscle}).(conditions{condition});
            Results.left.noise.walking_speed_left.(muscles{muscle}).(conditions{condition})= ((current_mean_freq_stride)./alpha).^(1/beta);
        end
    end
end

% no-noise
for nbr=1:nbr_cond
    current_mean_freq_stride =  Results.left.no_noise.mean_stride_freq;
    Results.left.no_noise.walking_speed_left = ((current_mean_freq_stride)./alpha).^(1/beta);
end


% Right
% empirical values
alpha = 0.85;
beta = 0.5;
for muscle = 1: length(muscles)
    for condition = 1: length(conditions)
        for nbr=1:nbr_cond
            current_mean_freq_stride =  Results.right.noise.mean_stride_freq.(muscles{muscle}).(conditions{condition});
            Results.right.noise.walking_speed_right.(muscles{muscle}).(conditions{condition}) =  ((current_mean_freq_stride)./alpha).^(1/beta);
        end
    end
end


% no-noise
for nbr=1:nbr_cond
    current_mean_freq_stride =  Results.right.no_noise.mean_stride_freq;
    Results.right.no_noise.walking_speed_right = ((current_mean_freq_stride)./alpha).^(1/beta);
end
%% Stride length

% Left- noise
for muscle = 1: length(muscles)
    for condition = 1: length(conditions)
        current_mean_duration_stride =  2./(Results.left.noise.mean_stride_freq.(muscles{muscle}).(conditions{condition}));
        current_mean_speed_stride = Results.left.noise.walking_speed_left.(muscles{muscle}).(conditions{condition});
        Results.left.noise.mean_length_stride.(muscles{muscle}).(conditions{condition}) = current_mean_speed_stride * current_mean_duration_stride;
    end
end

% no noise
current_mean_duration_stride =  2./(Results.left.no_noise.mean_stride_freq);
current_mean_speed_stride = Results.left.no_noise.walking_speed_left;
Results.left.no_noise.mean_length_stride = current_mean_speed_stride * current_mean_duration_stride;

% Right
for muscle = 1: length(muscles)
    for condition = 1: length(conditions)
        current_mean_duration_stride =  2./(Results.right.noise.mean_stride_freq.(muscles{muscle}).(conditions{condition}));
        current_mean_speed_stride = Results.right.noise.walking_speed_right.(muscles{muscle}).(conditions{condition});
        Results.right.noise.mean_length_stride.(muscles{muscle}).(conditions{condition}) = current_mean_speed_stride * current_mean_duration_stride;
    end
end

% no noise
current_mean_duration_stride =  2./(Results.right.no_noise.mean_stride_freq);
current_mean_speed_stride = Results.right.no_noise.walking_speed_right;
Results.right.no_noise.mean_length_stride = current_mean_speed_stride * current_mean_duration_stride;


%% We are interested in the stride length:

% No noise
Stride_length.no_noise_left = Results.left.no_noise.mean_length_stride;
Stride_length.no_noise_righ = Results.right.no_noise.mean_length_stride;

% Noise - left TA
Stride_length.noise_left_TA_s_f = Results.left.noise.mean_length_stride.TA.symetric_f;
Stride_length.noise_left_TA_a_f = Results.left.noise.mean_length_stride.TA.asymetric_f;
Stride_length.noise_left_TA_s_s = Results.left.noise.mean_length_stride.TA.symetric_s;
Stride_length.noise_left_TA_a_s = Results.left.noise.mean_length_stride.TA.asymetric_s;
% Noise - right TA
Stride_length.noise_right_TA_s_f = Results.right.noise.mean_length_stride.TA.symetric_f;
Stride_length.noise_right_TA_a_f = Results.right.noise.mean_length_stride.TA.asymetric_f;
Stride_length.noise_right_TA_s_s = Results.right.noise.mean_length_stride.TA.symetric_s;
Stride_length.noise_right_TA_a_s = Results.right.noise.mean_length_stride.TA.asymetric_s;

% Noise - left HAM
Stride_length.noise_left_HAM_s_f = Results.left.noise.mean_length_stride.HAM.symetric_f;
Stride_length.noise_left_HAM_a_f = Results.left.noise.mean_length_stride.HAM.asymetric_f;
Stride_length.noise_left_HAM_s_s = Results.left.noise.mean_length_stride.HAM.symetric_s;
Stride_length.noise_left_HAM_a_s = Results.left.noise.mean_length_stride.HAM.asymetric_s;
% Noise - right HAM
Stride_length.noise_right_HAM_s_f = Results.right.noise.mean_length_stride.HAM.symetric_f;
Stride_length.noise_right_HAM_a_f = Results.right.noise.mean_length_stride.HAM.asymetric_f;
Stride_length.noise_right_HAM_s_s = Results.right.noise.mean_length_stride.HAM.symetric_s;
Stride_length.noise_right_HAM_a_s = Results.right.noise.mean_length_stride.HAM.asymetric_s;

%% We could also be interested in the speed

% No noise
Walking_speed.no_noise_left = Results.left.no_noise.walking_speed_left;
Walking_speed.no_noise_right = Results.right.no_noise.walking_speed_right;

% Noise - left TA
Walking_speed.noise_left_TA_s_f = Results.left.noise.walking_speed_left.TA.symetric_f;
Walking_speed.noise_left_TA_a_f = Results.left.noise.walking_speed_left.TA.asymetric_f;
Walking_speed.noise_left_TA_s_s = Results.left.noise.walking_speed_left.TA.symetric_s;
Walking_speed.noise_left_TA_a_s = Results.left.noise.walking_speed_left.TA.asymetric_s;
% Noise - right TA
Walking_speed.noise_right_TA_s_f = Results.right.noise.walking_speed_right.TA.symetric_f;
Walking_speed.noise_right_TA_a_f = Results.right.noise.walking_speed_right.TA.asymetric_f;
Walking_speed.noise_right_TA_s_s = Results.right.noise.walking_speed_right.TA.symetric_s;
Walking_speed.noise_right_TA_a_s = Results.right.noise.walking_speed_right.TA.asymetric_s;

% Noise - left HAM
Walking_speed.noise_left_HAM_s_f = Results.left.noise.walking_speed_left.HAM.symetric_f;
Walking_speed.noise_left_HAM_a_f = Results.left.noise.walking_speed_left.HAM.asymetric_f;
Walking_speed.noise_left_HAM_s_s = Results.left.noise.walking_speed_left.HAM.symetric_s;
Walking_speed.noise_left_HAM_a_s = Results.left.noise.walking_speed_left.HAM.asymetric_s;
% Noise - right HAM
Walking_speed.noise_right_HAM_s_f = Results.right.noise.walking_speed_right.HAM.symetric_f;
Walking_speed.noise_right_HAM_a_f = Results.right.noise.walking_speed_right.HAM.asymetric_f;
Walking_speed.noise_right_HAM_s_s = Results.right.noise.walking_speed_right.HAM.symetric_s;
Walking_speed.noise_right_HAM_a_s = Results.right.noise.walking_speed_right.HAM.asymetric_s;


