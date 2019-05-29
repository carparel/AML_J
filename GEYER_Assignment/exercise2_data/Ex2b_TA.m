%% AML-Reflex Model Project
% Authors:
% Gaia Carparelli
% Marion Claudet
clc;
clear all;
close all;
%% Run simulation
% Uncomment only if your really want to re-do the simulation!
init();

%% Symetric feedback
struct_mRFX_TA_symetric_feedback = mRFX;
save('struct_mRFX_TA_symetric_feedback.mat','struct_mRFX_TA_symetric_feedback'); 

%% Asymetric feedback
struct_mRFX_TA_asymetric_feedback = mRFX;
save('struct_mRFX_TA_asymetric_feedback.mat','struct_mRFX_TA_asymetric_feedback'); 

%% Symetric stimulation
struct_mRFX_TA_symetric_stimulation = mRFX;
save('struct_mRFX_TA_symetric_stimulation.mat','struct_mRFX_TA_symetric_stimulation'); 

%% Asymetric stimulation
struct_mRFX_TA_asymetric_stimulation = mRFX;
save('struct_mRFX_TA_asymetric_stimulation.mat','struct_mRFX_TA_asymetric_stimulation'); 
%% Symetric feedback low noise
struct_mRFX_TA_symetric_feedback_low = mRFX;
save('struct_mRFX_TA_symetric_feedback_low.mat','struct_mRFX_TA_symetric_feedback_low'); 

%% Asymetric feedback low noise
struct_mRFX_TA_asymetric_feedback_low = mRFX;
save('struct_mRFX_TA_asymetric_feedback_low.mat','struct_mRFX_TA_asymetric_feedback_low'); 

%% Symetric stimulation low noise
struct_mRFX_TA_symetric_stimulation_low = mRFX;
save('struct_mRFX_TA_symetric_stimulation_low.mat','struct_mRFX_TA_symetric_stimulation_low'); 

%% Asymetric stimulation low noise
struct_mRFX_TA_asymetric_stimulation_low = mRFX;
save('struct_mRFX_TA_asymetric_stimulation_low.mat','struct_mRFX_TA_asymetric_stimulation_low'); 