function exe_3_2(dx_selected, angAtt_selected, k_selected)
% exe_3_2(dx_selected, angAtt_selected, k_selected)
% Inputs:
%   dx_selected: the forward speed of your solution (m/s)
%   angAtt_selected: the angle of attack of your solution (deg)
%   k_selected: the spring stiffness of your solution

% EXE-3  Extracting return map (Poincare map)

% *************************************************************************
% This tutorial is based on open-source Gait-Creation Framework published
% by David Remy et. al., 2011. 
% 'A MATLAB Framework For Gait Creation', 2011, C. David Remy (1), Keith
%  Buffinton (2), and Roland Siegwart (1),  International Conference on
%  Intelligent Robots and Systems, September 25-30, San Francisco, USA
% For more information please see
% http://www.asl.ethz.ch/people/cremy/personal/GaitCreation
%
% Analysis and Moldeling of Locomotion Course
% SLIP: Spring Loaded Inverted Pendulum Model
% Assignment-1, Exercise-3_2
% Biorobotics laboratoty, EPFL
% Salman Faraji (salman.faraji@epfl.ch)
% Spring 2014
% *************************************************************************

% The goal of this exercise is to:  
% 1) learn how you could extract the return map for this system
% 2) How to use this map to find the region of attraction where the system
% shows stable periodic solutions

% Return map: the state at the end of the stride vs. the state at the
% beginning of the stride
 
% initialize files and paths
initSimulation;

global slope
slope = 0.0; % the tangent of the ground

% Initialize the simulation parameters
yCYC = ContStateDefinition;
zCYC = DiscStateDefinition;
pCYC = SystParamDefinition;


disp('==========================================================================');
disp('This function computes the return map of y variable for the given system');
fprintf('\n');
disp('                 **** Important Message For You! ****  ')
disp('You need to show us a stable solution with 20 successful steps to proceed! ')


% step-1: compute the return maps
yCYC(contStateIndices.dx)   = dx_selected;
pCYC(systParamIndices.angAtt) = (90-angAtt_selected) * pi/180;
pCYC(systParamIndices.k)      = k_selected; 


% Define options for the simulation:
simOptions.tIN  = 0;  
simOptions.tMAX = 5; 


% The total energy of this gait: 0.5*m0*dx^2 + m0*g*y
E_tot = 0.5*pCYC(systParamIndices.m_0)*yCYC(contStateIndices.dx)^2 + ...
      pCYC(systParamIndices.m_0)*pCYC(systParamIndices.g)*yCYC(contStateIndices.y);

% We now vary the forward velocity dx and the hopping height y, such that
% the energy content of the system dose not change

% number of sample points
n_samples = 50;

fprintf('\n')    
disp('      =============== Computing Return Maps : ============')          
disp(' ==> Select one of the periodic solutions you found in previous execises ')         
disp(['                 number of samples: [',num2str(n_samples), ']']) 
disp('============================================================================')


% Variations in forward velocity:
dx_in = linspace(0.9*dx_selected, 1.1*dx_selected, n_samples);

% Energy equivalent change in hopping height:
y_in  = (E_tot - (0.5*pCYC(systParamIndices.m_0)*dx_in.^2))./pCYC(systParamIndices.g)./pCYC(systParamIndices.m_0);

DX_in = []; % make a copy to ignore unvalid results
dx_out = [];
y_out = [];
for i = 1:n_samples;
    yIN = yCYC;
    yIN(contStateIndices.dx) = dx_in(i);
    yIN(contStateIndices.y) = y_in(i);
    yOUT = HybridDynamics2(yIN, zCYC, pCYC, simOptions);
    if yOUT(contStateIndices.y)>0
        DX_in = [DX_in dx_in(i)];
        dx_out = [dx_out yOUT(contStateIndices.dx)];
        y_out  = [y_out yOUT(contStateIndices.y)];
        disp(['Iteration: ',num2str(i), '  ==>  dx_in:', num2str(dx_in(i)), ...
          '   dx_out:', num2str(dx_out(i))]);
    end

end
dx_in = DX_in;

% step-2: plot dx return-map
% Plot the first order return map of the hopper as a function of dx:
% Plot the first order return map of the hopper as a function of dx:
fig = figure;
pos = get(fig, 'position');
set(fig, 'position', [pos(1:3) pos(4)*2]);
subplot(2,1,1);
hold on
grid on
box on
plot(dx_in, dx_out, 'b','LineWidth',2)

% All periodic solutions are on this line:
line(dx_in, dx_in, 'color', 'r','LineWidth',2)
%axis equal
%axis tight
xlabel('dx before stride','FontWeight','bold','FontSize',18,...
    'FontName','Times New Roman')
ylabel('dx after stride','FontWeight','bold','FontSize',18,...
    'FontName','Times New Roman')
legend('Periodicity Line (dx_{in}=dx_{out})', 'Real input-output function')


% step-2: analyze dx return-map
fprintf('\n')    
disp('================== Now analyze "dx" Return Map : ============')                   
disp('==> HINT-1: check the slope of the return map at the fixed point (shown in red!) ')  
disp('==> HINT-2: now see the eigenvalues of the fixed point in the second graph')  
disp('Question: how could you analyze the satbility of the fixed points using this information? ')
fprintf('\n') 

% Here we process the results from the graphs by some graphical analysis
% methods; including slope at fixed points (== eigenvalues)

% first use spline to have more sample data for the graph (using spline)
n_sample     = 1000;
dx_in_finer   = linspace(dx_in(1),dx_in(end),n_sample);
dx_out_spline = spline(dx_in, dx_out);
dx_out_finer  = ppval(dx_out_spline, dx_in_finer);

% find the fix points and their indices
[~, dx_fixed_points_ind] = find(abs(dx_out_finer - dx_in_finer) <= 1e-3);
dx_fixed_points = dx_in_finer(dx_fixed_points_ind);

plot(dx_in_finer(dx_fixed_points_ind), dx_out_finer(dx_fixed_points_ind), ...
      '*g', 'MarkerSize', 10, 'LineWidth', 2)

% find the slope at the fix points
dx_slope_all = diff(dx_out_finer) ./ diff(dx_in_finer);
dx_slope_fix_points = dx_slope_all(dx_fixed_points_ind);

subplot(2,1,2);
plot(dx_in(1:end-1),diff(dx_out)./diff(dx_in),'LineWidth', 2)
hold on
grid on
box on
xlabel('dx before stride','FontWeight','bold','FontSize',18,...
        'FontName','Times New Roman')
ylabel('Eigenvalue','FontWeight','bold','FontSize',18,...
        'FontName','Times New Roman')

line(dx_in, ones(size(dx_in)),'LineWidth', 2, 'color', 'r')

plot(dx_in_finer(dx_fixed_points_ind), dx_slope_fix_points, ...
      '*g', 'MarkerSize', 10, 'LineWidth', 2)

legend('Eigenvalues of dx return map', 'Stability border: Eigenvalue = 1')

if isempty(diff(dx_out)./diff(dx_in) < 1)
    disp('                 **** Important Message For You! ****  ')
    disp('No stable fixed point!! You need to restart with a stable solution! ')

end

disp('======================================================================================');
