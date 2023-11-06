#include <stdlib.h>
#include <math.h>
#define n 2
#define tolerance .01
float A[n+2][n+2], B[n+2][n+2];
int i,j,count;
float change, maxchange;
boolean globaldone;
spinlock Arrival, Departure;

void Barrier(int me) {
 /*Arrival Phase - Count processes as they arrive*/
 Lock(Arrival);
 count = count + 1;
 if (count < n) 
 Unlock(Arrival); /*continue Arrival Phase*/
 else Unlock(Departure); /*end Arrival Phase*/
 /*Departure Phase - Count processes as they leave*/
 Lock(Departure);
 count = count - 1;
 if (count > 0) 
 Unlock(Departure); /*continue Departure Phase*/
 else Unlock(Arrival); /*end Departure Phase*/
}

boolean Aggregate(boolean mydone) {
 boolean result;
 /*Arrival Phase - Count the processes arriving*/
 Lock(Arrival);
 count = count + 1;
 globaldone = globaldone && mydone; /*aggregation*/
 if (count < n)
 Unlock(Arrival); /*continue Arrival Phase*/
 else Unlock(Departure); /*end Arrival Phase*/
 /*Departure Phase - Count the processes leaving*/
 Lock(Departure);
 count = count - 1;
 result = globaldone; /*return “done” flag*/
 if (count > 0)
 Unlock(Departure); /*continue Departure Phase*/
 else {
 Unlock(Arrival); /*terminate Departure Phase*/
 globaldone = true; /*reset for new Aggregation*/
 }
 return(result);
}

main( ){
count = 0;
cout.precision(2);
globaldone=true;
Unlock(Arrival);
Lock(Departure);
cout<<"Input"<<endl;
 for (i = 0; i <= n+1; i++) 
 { 
 for (j = 0; j <= n+1; j++)
 cin >> A[i][j];
 }
 cout<<endl;
B = A;
forall i = 1 to n do { /*Create the processes*/
 int j;
 float change, maxchange;
 boolean done;
 do {
 maxchange = 0;
 for (j = 1; j <= n; j++) {
 /*compute new value for each point in my row*/
 B[i][j] = (A[i-1][j] + A[i+1][j] + 
 A[i][j-1] + A[i][j+1]) / 4;
 change = fabs( B[i][j] - A[i][j] );
 if (change > maxchange) maxchange = change;
 }
 Barrier(i-1);
 A[i] = B[i];
 done = Aggregate(maxchange<tolerance);
 }while (!done); /*iterate until global termination*/
}
cout<<"Output"<<endl;
for (i = 0; i <= n+1; i++) 
 { 
 for (j = 0; j <= n+1; j++)
 cout << A[i][j];
 cout<<endl;
 }
}