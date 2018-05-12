


% first read in all predictions in prediction directory

pfiles = dir('predictions*.dta')
A = NaN;
for file = pfiles'
    p = load(file);
    if file == 'truth.dta'
        truth = p;
    elseif isnan(A)
        A = p;
    else
        A = [A p];
    end
end
weights = inv(A'*A)*A'*truth;
disp(weights);

wpred = A*weights;

fileID = fopen('blendpred.dta','w');
fprintf(fileID,'%f\n', wpred);
fclose(fileID);
