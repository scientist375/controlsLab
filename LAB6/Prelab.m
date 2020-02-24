% actuator quantization (2000 duty cycles for +/- 24 V)
Qa = 48/2000;
Xa = 0.4;
xa = Xa*(-1:1/1000:1);
ya = Qa*round(xa/Qa);
subplot(121), plot([-Xa Xa],[0 0],'k',[0 0],[-Xa Xa],'k')
hold on, stairs(xa,ya,'LineWidth',1.5), hold off
axis([-Xa Xa -Xa Xa]), axis square
xlabel('desired voltage [V]'), ylabel('actual voltage [V]')
title('actuator quantization')
% sensor quantization (1000 counts per revolution)
Qs = 2*pi/1000;
Xs = 0.1;
xs = Xs*(-1:1/1000:1);
ys = Qs*round(xs/Qs);
subplot(122), plot([-Xs Xs],[0 0],'k',[0 0],[-Xs Xs],'k')
hold on, stairs(xs,ys,'LineWidth',1.5), hold off
axis([-Xs Xs -Xs Xs]), axis square
xlabel('actual position [rad]'), ylabel('sensed position [rad]')
title('sensor quantization')
