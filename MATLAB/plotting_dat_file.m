% Configuration
datFilePath = '01000_lr.dat';
numberOfSignals = 12;
signalIndexLeadII = 3; % Lead selection
maxSamples = 1000;

% Read the binary file
fileID = fopen(datFilePath, 'r');
data = fread(fileID, inf, 'int16');
fclose(fileID);

% Reshape the data to separate each signal
data = reshape(data, numberOfSignals, []);

% Extract Lead II signal
leadIISignal = data(signalIndexLeadII, :);

% Trim the signal to maxSamples
leadIISignal = leadIISignal(1:min(maxSamples, length(leadIISignal)));

wave=leadIISignal(421:514);

for ii=1:5
wave=[wave leadIISignal(421:500)];
end
% Plot the original Lead II signal
 figure;
 plot(wave,'Marker','.');
 hold on
% subplot(2, 1, 1);
 plot(leadIISignal(421:920),'Marker','+');
 title('Original Lead I Signal');
 xlabel('Sample Number');
 ylabel('Amplitude');
 grid on