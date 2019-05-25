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
struct_mRFX_HAM_symetric_feedback = mRFX;
save('struct_mRFX_HAM_symetric_feedback.mat','struct_mRFX_HAM_symetric_feedback'); 

%% Asymetric feedback
struct_mRFX_HAM_asymetric_feedback = mRFX;
save('struct_mRFX_HAM_asymetric_feedback.mat','struct_mRFX_HAM_asymetric_feedback'); 

%% Symetric stimulation
struct_mRFX_HAM_symetric_stimulation = mRFX;
save('struct_mRFX_HAM_symetric_stimulation.mat','struct_mRFX_HAM_symetric_stimulation'); 

%% Asymetric stimulation
struct_mRFX_HAM_asymetric_stimulation = mRFX;
save('struct_mRFX_HAM_asymetric_stimulation.mat','struct_mRFX_HAM_asymetric_stimulation'); 