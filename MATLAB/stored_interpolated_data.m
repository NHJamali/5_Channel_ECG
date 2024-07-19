% Configuration
datFilePath = '01000_lr.dat';
heaFilePath = '01000_lr.hea';
numberOfSignals = 12;
signalIndexLeadII = 2;
maxSamples = 1000;

% Read the binary file
fileID = fopen(datFilePath, 'r');
data = fread(fileID, inf, 'int16');
fclose(fileID);

leadSignal=zeros(5,1000);
new_data=zeros(5,173);


% Reshape the data to separate each signal
data = reshape(data, numberOfSignals, []);

leadindex=[1,2,3,7,8];

for i=1:length(leadindex)

    % Extract Lead signal
    leadSignal(i,:) = data(leadindex(i), :);

    % Perform the interpolation 
    samples_ind = 255:341;
    samples_val = leadSignal(i,255:341);
    interpolatedIndices = 255:0.5:341;
    new_data(i,:) = interp1(samples_ind, samples_val, interpolatedIndices, 'linear');

end

% Write the new data to a .dat file
newDatFilePath = 'interpolated_01000_lr.dat';
fileID = fopen(newDatFilePath, 'w');
fwrite(fileID, new_data(:), 'int16');
fclose(fileID);

disp(['Interpolated data saved to ', newDatFilePath]);


%%% Verification

% Configuration
Signals = 5;
signalIndexLeadII = 2; 
maxSamples = 173;

% Read the binary file
fileID = fopen(newDatFilePath, 'r');
inter_data = fread(fileID, inf, 'int16');
fclose(fileID);

% Reshape the data to separate each signal
inter_data = reshape(inter_data, Signals, []);


figure
plot(255:0.5:341,inter_data(1,:),'Marker','.')
hold on
% 
% figure;
% % subplot(2, 1, 1);
 plot(255:341,(data(1,255:341)),'Marker','o');
 title('Lead I Signal');
 xlabel('Sample Number');
 ylabel('Amplitude');
 grid on

 figure
plot(255:0.5:341,inter_data(2,:),'Marker','.')
hold on
% 
% figure;
% % subplot(2, 1, 1);
 plot(255:341,(data(2,255:341)),'Marker','o');
 title('Lead II Signal');
 xlabel('Sample Number');
 ylabel('Amplitude');
 grid on


  figure
plot(255:0.5:341,inter_data(3,:),'Marker','.')
hold on
% 
% figure;
% % subplot(2, 1, 1);
 plot(255:341,(data(3,255:341)),'Marker','o');
 title('Lead III Signal');
 xlabel('Sample Number');
 ylabel('Amplitude');
 grid on

  figure
plot(255:0.5:341,inter_data(4,:),'Marker','.')
hold on
% 
% figure;
% % subplot(2, 1, 1);
 plot(255:341,(data(7,255:341)),'Marker','o');
 title('Lead V1 Signal');
 xlabel('Sample Number');
 ylabel('Amplitude');
 grid on


  figure
plot(255:0.5:341,inter_data(5,:),'Marker','.')
hold on
% 
% figure;
% % subplot(2, 1, 1);
 plot(255:341,(data(8,255:341)),'Marker','o');
 title('Lead V2 Signal');
 xlabel('Sample Number');
 ylabel('Amplitude');
 grid on