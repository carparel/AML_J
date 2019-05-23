%% AML-Reflex Model Project
% Authors:
% Gaia Carparelli
% Marion Claudet
clc;
clear all;
close all;
%% Run simulation
% Uncomment only if your really want to re-do the simulation!
% init();

% After running init simulation
%struct_mRFX = mRFX;
%save('struct_mRFX.mat','struct_mRFX'); % to use it later

%% If you have not re-run the init simulation already

load('struct_mRFX.mat');

%% Extract heel off indices (to split into gait cycles

indices_stance_left = find(struct_mRFX.LStance == 1);
indices_stance_right = find(struct_mRFX.RStance == 1);

% 1 --> stance, 0 --> swing
% 1 stride --> first 1 to first 1 (next serie) --> indices to split gait
HS_left = [];
HS_right = [];

% For the left foot
for i = 1:length(indices_stance_left)-1
    
    if (indices_stance_left(i+1) - indices_stance_left(i) > 1)
        HS_left = [HS_left, indices_stance_left(i+1)];
    end
end
% For the right foot
for i = 1:length(indices_stance_right)-1
    if (indices_stance_right(i+1) - indices_stance_right(i) > 1)
        HS_right = [HS_right, indices_stance_right(i+1)];
    end
end
%% Extract the muscles activities
muscles = {'MuscleGLU','MuscleHF','MuscleHAM','MuscleVAS','MuscleGAS','MuscleSOL','MuscleTA'};
% Left foot
for muscle = 1: length(muscles)
    struct_muscles.left.whole.(muscles{muscle}) = struct_mRFX.(['L' (muscles{muscle})]);
end

for i = 1:length(HS_left)-1
    for muscle = 1: length(muscles)
        old_activity = struct_muscles.left.whole.(muscles{muscle});
        struct_muscles.left.parsed.(muscles{muscle}){i}= old_activity(HS_left(i):HS_left(i+1));
        
    end
end

% Right foot

for muscle = 1: length(muscles)
    struct_muscles.right.whole.(muscles{muscle}) = struct_mRFX.(['R' (muscles{muscle})]);
end

for i = 1:length(HS_right)-1
    for muscle = 1: length(muscles)
        old_activity = struct_muscles.right.whole.(muscles{muscle});
        struct_muscles.right.parsed.(muscles{muscle}){i} = old_activity(HS_right(i):HS_right(i+1));
    end
end

%% Extract joint angles and joint torques per stride

% Extracting angles
angles = {'AnkleAngle','KneeAngle','HipAngle'};

% Left foot
for angle = 1: length(angles)
    struct_angles.left.whole.(angles{angle}) = struct_mRFX.(['L' (angles{angle})]);
end
for i = 1:length(HS_left)-1
    for angle = 1: length(angles)
        old_activity = struct_angles.left.whole.(angles{angle});
        struct_angles.left.parsed.(angles{angle}){i}= old_activity(HS_left(i):HS_left(i+1));
        
    end
end

% Right foot
for angle = 1: length(angles)
    struct_angles.right.whole.(angles{angle}) = struct_mRFX.(['R' (angles{angle})]);
end
for i = 1:length(HS_right)-1
    for angle = 1: length(angles)
        old_activity = struct_angles.right.whole.(angles{angle});
        struct_angles.right.parsed.(angles{angle}){i}= old_activity(HS_right(i):HS_right(i+1));
        
    end
end
% Extracting torques
torques = {'AnkleTorque','KneeTorque','HipTorque'};

for torque = 1: length(torques)
    struct_torques.left.whole.(torques{torque}) = struct_mRFX.(['L' (torques{torque})]);
end
for i = 1:length(HS_left)-1
    for torque  = 1: length(torques)
        old_activity = struct_torques.left.whole.(torques{torque});
        struct_torques.left.parsed.(torques{torque}){i}= old_activity(HS_left(i):HS_left(i+1));
        
    end
end

% Right foot
for torque  = 1: length(torques)
    struct_torques.right.whole.(torques{torque}) = struct_mRFX.(['R' (torques{torque})]);
end
for i = 1:length(HS_right)-1
    for torque = 1: length(torques)
        old_activity = struct_torques.right.whole.(torques{torque});
        struct_torques.right.parsed.(torques{torque}){i}= old_activity(HS_right(i):HS_right(i+1));
        
    end
end