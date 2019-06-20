#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define Q_LIMIT 100
#define BUSY 1
#define IDLE 0
int next_event_type, num_custs_delayed, num_delays_required,
    num_events, num_in_q, server_status;
float area_num_in_q, area_server_status, mean_interarrival,
      mean_service, time, time_arrival[Q_LIMIT + 1],
      time_last_event, time_next_event[3], total_of_delays;
FILE *infile, *outfile;
void initialize(void);
void timing(void);
void arrive(void);
void depart(void);
void report(void);
void update_time_avg_stats(void);
float expon(float mean);

int main()
{
    infile = fopen("mm1.in", "r");
    outfile = fopen("mm1.out", "w");
    fscanf(infile, "%f %f %d", &mean_interarrival, &mean_service, &num_delays_required);
    fprintf(outfile, "Single-server queuing system \n\n");
    fprintf(outfile, "Mean interarrival %11.3f  minutes\n\n", mean_interarrival);
    fprintf(outfile, "Mean service time %16.3f minutes\n\n", mean_service);
    fprintf(outfile, "Number of customers %14d\n\n", num_delays_required);
    initialize();
    while (num_custs_delayed < num_delays_required)
    {
        timing();
        update_time_avg_stats();
        switch (next_event_type)
        {
        case 1:
            arrive();
            break;
        case 2:
            depart();
            break;
        }
    }
    report();
    fclose(infile);
    fclose(outfile);
    return 0;
}
void initialize(void)
{

    time = 0.0;

    server_status = IDLE;
    num_in_q = 0;
    time_last_event = 0.0;

    num_custs_delayed = 0;
    total_of_delays = 0.0;
    area_num_in_q = 0.0;
    area_server_status = 0.0;

    time_next_event[1] = time + expon(mean_interarrival);
    time_next_event[2] = 1.0e+30;
}
void timing(void) 
{
    int i;
    float min_time_next_event = 1.0e+29;
    next_event_type = 0;

    for (i = 1; i <= num_events; ++i)
    {
        if (time_next_event[i] < min_time_next_event)
        {
            min_time_next_event = time_next_event[i];
            next_event_type = i;
        }
    }
    if (next_event_type == 0)
    {
        fprintf(outfile, "\nEvent list empty at time %f", time);
        exit(1);
    }
    time = min_time_next_event;
}
void arrive(void)
{
    float delay;
    time_next_event[1] = time + expon(mean_interarrival);

    if (server_status == BUSY)
    {
        ++num_in_q;

        if (num_in_q > Q_LIMIT)
        {
            fprintf(outfile, "\nOverflow of the array time_arrival at");
            fprintf(outfile, " time %f", time);
            exit(2);
        }

        time_arrival[num_in_q] = time;
    }
    else
    {

        delay = 0.0;
        total_of_delays += delay;

        ++num_custs_delayed;
        server_status = BUSY;

        time_next_event[2] = time + expon(mean_service);
    }
}
void depart(void)
{
    int i;
    float delay;

    if (num_in_q == 0)
    {
        server_status = IDLE;
        time_next_event[2] = 1.0e+30;
    }
    else
    {
        --num_in_q;
        delay = time - time_arrival[1];
        total_of_delays += delay;
        ++num_custs_delayed;
        time_next_event[2] = time + expon(mean_service);
    }
    for (i = 1; i <= num_in_q; ++i)
        time_arrival[i] = time_arrival[i + 1];
}
void report(void)
{
    fprintf(outfile, "\n\nAverage delay in queue%11.3f minutes\n\n", total_of_delays / num_custs_delayed);
    fprintf(outfile, "Average number in queue%10.3f\n\n", area_num_in_q / time);
    fprintf(outfile, "Server utilization%15.3f\n\n", area_server_status / time);
    fprintf(outfile, "Time simulation ended%12.3f", time);
}
void update_time_avg_stats(void)
{
    float time_since_last_event;
    time_since_last_event = time - time_last_event;
    time_last_event = time;
    area_num_in_q += num_in_q * time_since_last_event;
    area_server_status += server_status * time_since_last_event;
}
float expon(float mean)
{
    float u;
    u = rand();
    return -mean * log(u);
}
