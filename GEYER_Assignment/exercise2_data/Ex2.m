%% AML-Reflex Model Project
% Authors:
% Gaia Carparelli
% Marion Claudet
clc;
clear all;
close all;
%% Run simulation
% Uncomment only if your really want to re-do the simulation!
%init();

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

%% Muscles activity-interpolation
nbr_points = 1000; % for interpolation
% Right foot
for i = 1:length(HS_right)-1
    for muscle = 1: length(muscles)
        current_signal = struct_muscles.right.parsed.(muscles{muscle}){i};
        interpolated_signal = interp1(1:1:length(current_signal),current_signal,1:1:nbr_points,'nearest');
        interpolated_signal_per.right.(muscles{muscle}){i} = interpolated_signal /nbr_points * 100; % to have gait cycles
    end
end
% Left foot
for i = 1:length(HS_left)-1
    for muscle = 1: length(muscles)
        current_signal = struct_muscles.left.parsed.(muscles{muscle}){i};
        interpolated_signal= interp1(1:1:length(current_signal),current_signal,1:1:nbr_points,'nearest');
        interpolated_signal_per.left.(muscles{muscle}){i} = interpolated_signal/nbr_points * 100; % to have gait cycles
    end
end

% Meand and std
% Right
for i = 1:length(HS_right)-1
    for muscle = 1:length(muscles)
        muscles_matrix_right.(muscles{muscle})(i,:) = interpolated_signal_per.right.(muscles{muscle}){i};
    end
end
for muscle = 1:length(muscles)
    mean_muscles_right.(muscles{muscle}) = mean(muscles_matrix_right.(muscles{muscle}));
    std_muscles_right.(muscles{muscle}) = std(muscles_matrix_right.(muscles{muscle}));
end
% Left
for i = 1:length(HS_left)-1
    for muscle = 1:length(muscles)
        muscles_matrix_left.(muscles{muscle})(i,:) = interpolated_signal_per.left.(muscles{muscle}){i};
    end
end
for muscle = 1:length(muscles)
    mean_muscles_left.(muscles{muscle}) = mean(muscles_matrix_left.(muscles{muscle}));
    std_muscles_left.(muscles{muscle}) = std(muscles_matrix_left.(muscles{muscle}));
end

%% Plotting Muscles

% Right muscles
for muscle = 1: length(muscles)
    indices = 1:1:length(mean_muscles_right.(muscles{muscle}));
    figure;
    plot(mean_muscles_right.(muscles{muscle}),'k','LineWidth',1.2);
    fill_x = [indices,fliplr(indices)];
    fill_y = [mean_muscles_right.(muscles{muscle}) - std_muscles_right.(muscles{muscle}),fliplr((mean_muscles_right.(muscles{muscle}) + std_muscles_right.(muscles{muscle})))];
    patch(fill_x,fill_y,'r','EdgeColor','none');
    alpha(.1)
    ax = gca;
    ax.FontSize = 15;
    legend('Average','Std');
    title({'Average activity for right',num2str(muscles{muscle})});
    saveas(gcf,['Activity_right', num2str(muscles{muscle})],'png');
end
% Left muscles
for muscle = 1: length(muscles)
    indices = 1:1:length(mean_muscles_left.(muscles{muscle}));
    figure;
    plot(mean_muscles_left.(muscles{muscle}),'k','LineWidth',1.2);
    fill_x = [indices,fliplr(indices)];
    fill_y = [mean_muscles_left.(muscles{muscle}) - std_muscles_left.(muscles{muscle}),fliplr((mean_muscles_left.(muscles{muscle}) + std_muscles_left.(muscles{muscle})))];
    patch(fill_x,fill_y,'r','EdgeColor','none');
    alpha(.1)
    ax = gca;
    ax.FontSize = 15;
    
    legend('Average','Std');
    title({'Average activity for left ',num2str(muscles{muscle})});
    saveas(gcf,['Activity_left', num2str(muscles{muscle})],'png');
end

%% Angles-interpolation
% Right foot
for i = 1:length(HS_right)-1
    for angle = 1: length(angles)
        current_signal = struct_angles.right.parsed.(angles{angle}){i};
        interpolated_signal = interp1(1:1:length(current_signal),current_signal,1:1:nbr_points,'nearest');
        interpolated_signal_per.right.(angles{angle}){i} = interpolated_signal /nbr_points * 100; % to have gait cycles
    end
end
% Left foot
for i = 1:length(HS_left)-1
    for angle = 1: length(angles)
        current_signal = struct_angles.left.parsed.(angles{angle}){i};
        interpolated_signal = interp1(1:1:length(current_signal),current_signal,1:1:nbr_points,'nearest');
        interpolated_signal_per.left.(angles{angle}){i} = interpolated_signal /nbr_points * 100; % to have gait cycles
    end
end
% Meand and std
% Right
for i = 1:length(HS_right)-1
    for angle = 1: length(angles)
        angles_matrix_right.(angles{angle})(i,:) = interpolated_signal_per.right.(angles{angle}){i};
    end
end
for angle = 1: length(angles)
    mean_angles_right.(angles{angle}) = mean(angles_matrix_right.(angles{angle}));
    std_angles_right.(angles{angle}) = std(angles_matrix_right.(angles{angle}));
end
% Left
for i = 1:length(HS_left)-1
    for angle = 1: length(angles)
        angles_matrix_left.(angles{angle})(i,:) = interpolated_signal_per.left.(angles{angle}){i};
    end
end
for angle = 1: length(angles)
    mean_angles_left.(angles{angle}) = mean(angles_matrix_left.(angles{angle}));
    std_angles_left.(angles{angle}) = std(angles_matrix_left.(angles{angle}));
end
%% Plotting
% Right
for angle = 1: length(angles)
    indices = 1:1:length(mean_angles_right.(angles{angle}));
    figure;
    plot(mean_angles_right.(angles{angle}),'k','LineWidth',1.2);
    fill_x = [indices,fliplr(indices)];
    fill_y = [mean_angles_right.(angles{angle}) - std_angles_right.(angles{angle}),fliplr((mean_angles_right.(angles{angle}) + std_angles_right.(angles{angle})))];
    patch(fill_x,fill_y,'r','EdgeColor','none');
    alpha(.1)
    ax = gca;
    ax.FontSize = 15;
    title({'Average activity for right',num2str(angles{angle})});
    legend('Average','Std');
    saveas(gcf,['Activity_right', num2str(angles{angle})],'png');
end
% Left
for angle = 1: length(angles)
    indices = 1:1:length(mean_angles_left.(angles{angle}));
    figure;
    plot(mean_angles_left.(angles{angle}),'k','LineWidth',1.2);
    fill_x = [indices,fliplr(indices)];
    fill_y = [mean_angles_left.(angles{angle}) - std_angles_left.(angles{angle}),fliplr((mean_angles_left.(angles{angle}) + std_angles_left.(angles{angle})))];
    patch(fill_x,fill_y,'r','EdgeColor','none');
    alpha(.1)
    ax = gca;
    ax.FontSize = 15;
    title({'Average activity for left',num2str(angles{angle})});
    legend('Average','Std');
     saveas(gcf,['Activity_left', num2str(angles{angle})],'png');
end
%% Torques-interpolation
% Right foot
for i = 1:length(HS_right)-1
    for torque= 1: length(torques)
        current_signal = struct_torques.right.parsed.(torques{torque}){i};
        interpolated_signal = interp1(1:1:length(current_signal),current_signal,1:1:nbr_points,'nearest');
        interpolated_signal_per.right.(torques{torque}){i} = interpolated_signal /nbr_points * 100; % to have gait cycles
    end
end
% Left foot
for i = 1:length(HS_left)-1
    for torque= 1: length(torques)
        current_signal = struct_torques.left.parsed.(torques{torque}){i};
        interpolated_signal = interp1(1:1:length(current_signal),current_signal,1:1:nbr_points,'nearest');
        interpolated_signal_per.left.(torques{torque}){i} = interpolated_signal /nbr_points * 100; % to have gait cycles
    end
end
% Meand and std
% Right
for i = 1:length(HS_right)-1
    for torque= 1: length(torques)
        torques_matrix_right.(torques{torque})(i,:) = interpolated_signal_per.right.(torques{torque}){i};
    end
end
for torque= 1: length(torques)
    mean_torques_right.(torques{torque}) = mean(torques_matrix_right.(torques{torque}));
    std_torques_right.(torques{torque}) = std(torques_matrix_right.(torques{torque}));
end
% Left
for i = 1:length(HS_left)-1
    for torque= 1: length(torques)
        torques_matrix_left.(torques{torque})(i,:) = interpolated_signal_per.left.(torques{torque}){i};
    end
end
for torque= 1: length(torques)
    mean_torques_left.(torques{torque}) = mean(torques_matrix_left.(torques{torque}));
    std_torques_left.(torques{torque}) = std(torques_matrix_left.(torques{torque}));
end

%% Plotting
% Right
for torque= 1: length(torques)
    indices = 1:1:length(mean_torques_right.(torques{torque}));
    figure;
    plot(mean_torques_right.(torques{torque}),'k','LineWidth',1.2);
    fill_x = [indices,fliplr(indices)];
    fill_y = [mean_torques_right.(torques{torque}) - std_torques_right.(torques{torque}),fliplr((mean_torques_right.(torques{torque}) + std_torques_right.(torques{torque})))];
    patch(fill_x,fill_y,'r','EdgeColor','none');
    alpha(.1)
    ax = gca;
    ax.FontSize = 15;
    title({'Average activity for right',num2str(torques{torque})});
    legend('Average','Std');
    saveas(gcf,['Activity_right', num2str(torques{torque})],'png');
end
% Left
for torque= 1: length(torques)
    indices = 1:1:length(mean_torques_left.(torques{torque}));
    figure;
    plot(mean_torques_left.(torques{torque}),'k','LineWidth',1.2);
    fill_x = [indices,fliplr(indices)];
    fill_y = [mean_torques_left.(torques{torque}) - std_torques_left.(torques{torque}),fliplr((mean_torques_left.(torques{torque}) + std_torques_left.(torques{torque})))];
    patch(fill_x,fill_y,'r','EdgeColor','none');
    alpha(.1)
    ax = gca;
    ax.FontSize = 15;
    title({'Average activity for left',num2str(torques{torque})});
    legend('Average','Std');
    saveas(gcf,['Activity_left', num2str(torques{torque})],'png');
end





