# taskmaster (@42Paris)

> This project was code for Linux. Here is the [subject][1].
>

![Alt text](https://github.com/llefranc/42_taskmaster/blob/main/taskmaster_example.png)

## About

ft_ping is a reimplementation in C of the ping utility based on the ping implementation from inetutils-2.0.

ft_ping uses a raw socket to send each second an ICMP Echo Request packet with a timestamp in its body.
It will also display a line of information for each received ICMP packet.

It accepts a simple IPv4 address or hostname as a parameter, and supports both numerical IP addresses and hostnames.

> The program should be run with appropriate permissions, as sending ICMP Echo Request packets may require administrative privileges.

When receiving an ICMP packet, ft_ping will check:
- If the packet was correctly addressed to this process by checking the PID stored in the ID field of ICMP Echo Request.
- If it's an error packet or not.

ft_ping will ping the targeted host until `Ctrl+C` is pressed. It will then display statistics about the received ICMP Echo Response packets.

ft_ping supports also the following options :
- `-h`: provides help information about the usage and command-line options of ft_ping.
- `-q`: enables quiet output, ft_ping will only display the end statistics.
- `-v`: enables verbose output and allows viewing the results in case of problems or errors related to the packets.

*Example of error packet (packet filtered) with verbose output*

![Alt text](https://github.com/llefranc/42_ft_ping/blob/main/ft_ping_example2.png)

## Understanding the configuration file

![Alt text](https://github.com/llefranc/42_taskmaster/blob/main/taskmaster_config_file_example.png)

Taskmaster need a configuration file that will be loaded at the beginning of the program, which indicate how to manage the different processes. It works in a similar way than supervisor configuration file.

The configuration file contains one or many program blocks. Each program block defines the parameters for a group of process. Each program block are separated from others by empty lines.

Only `[programblockname]` and `cmd` lines are mandatory, the others lines are optionnal and will take default values if not provided. If there is several time the same line inside a programblock, then only the last line will be taken in account.

It will allow you to setup the following parameters for each process :

##### `[programblockname]`
The name use to identify the group of process. This line indicate the start of a new program block and must be separate from the previous program block by a newline.  
The programblockname must be unique and surrounded by brackets.

*Default: none*  
*Required: yes*  
*Value: minimum 1 character, maximum 20 characters*  

##### `cmd`
The absolute path to the executable to launch and the arguments if needed.  
**Warning: only absolute path, no relative path!**  

A non-correct path will not raise an error during the parsing, but the process launch will fail and the process will be put in a FATAL state (spawn failed).  

*Default: none*  
*Required: yes*  
*Value: minimum 1 character, maximum 1024*  

##### `numprocs`
The number of instances to start for a process. If superior to 1, the processes will be name the following way: "programblockname_x", where x is the instance number of the process.  

*Default: 1*  
*Required: no*  
*Value: an integer between 1 and 100*  

##### `umask`
An octal number (e.g. 002, 022) representing the umask of the process.

*Default: 022*
*Required: no*
*Value: octal value from 000 to 777*

##### `workingdir`
A file path representing the working directory of the process.
**Warning: only absolute path, no relative path!**

A non-correct path will not raise an error during the parsing, but the process launch will fail and the process will be put in a FATAL state (spawn failed).
*Default: working directory of taskmaster*
*Required: no*
*Value: minimum 1 character, maximum 1024*

##### `autostart`
If true, this program will start automatically when supervisord is started.
*Default: false*
*Required: no*
*Value: false, true*

##### `autorestart`
Specifies if taskmaster should automatically restart a process if it exits when it is in the `RUNNING` state. May be one of false, unexpected, or true. If false, the process will not be autorestarted. If unexpected, the process will be restarted when the program exits with an exit code that is not one of the exit codes associated with this process’ configuration (see `exitcodes`). If true, the process will be unconditionally restarted when it exits, without regard to its exit code.
*Default: false*
*Required: no*
*Value: false, unexpected, true*

##### `startretries`
The number of serial failure attempts that taskmaster will allow when attempting to start the program before giving up and putting the process into an `FATAL` state.
*Default: 0*
*Required: no*
*Value: an integer between 0 and 100*

##### `starttime`
The total number of seconds which the program needs to stay running after a startup to consider the start successful (moving the process from the `STARTING` state to the `RUNNING` state). Set to 0 to indicate that the program needn’t stay running for any particular amount of time.
*Default: 0*
*Required: no*
*Value: an integer between 0 and 86400*
> Even if a process exits with an “expected” exit code (see `exitcodes`), the start will still be considered a failure if the process exits quicker than `starttime`.

##### `exitcodes`
The list of “expected” exit codes for this program used with `autorestart`. If the `autorestart` parameter is set to unexpected, and the process exits in any other way than as a result of a taskmaster stop request, taskmaster will restart the process if it exits with an exit code that is not defined in this list.
*Default: 0*
*Required: no*
*Value: a list of integers between 0 and 255. Each exit code must be separated by ',' character with no spaces around ',' separator character.*

##### `stopsignal`
The signal used to kill the program when a stop is requested. This can be specified using the signal’s name.
*Default: TERM*
*Required: no*
*Value: INT, QUIT, KILL, USR1, USR2 and TERM*

##### `stoptime`
The number of seconds to wait for the OS to return a SIGCHLD to taskmaster after the program has been sent a `stopsignal`. If this number of seconds elapses before taskmaster receives a SIGCHLD from the process, taskmaster will attempt to kill it with a final SIGKILL.
*Default: 0*
*Required: no*
*Value: an integer between 0 and 86400*

##### `stdout`
Put process stdout output in this file.
**Warning: only absolute path, no relative path!**

A non-correct path will not raise an error during the parsing, but the process launch will fail and the process will be put in a FATAL state (spawn failed).
*Default: *programblockname_pid*_stdout.txt*
*Required: no*
*Value: minimum 1 character, maximum 1024*

##### `stderr`
Put process stderr output in this file.
**Warning: only absolute path, no relative path!**

A non-correct path will not raise an error during the parsing, but the process launch will fail and the process will be put in a FATAL state (spawn failed).
*Default: *programblockname_pid*_stderr.txt*
*Required: no*
*Value: minimum 1 character, maximum 1024*

##### `env`
A list of key/value pairs in the form KEY="val",KEY2="val2" that will be placed in the child process’ environment. Each environment variable must be separated by ',' character without any spaces around, and the value of the environment variable must be between double quotes.
*Default: empty list*
*Required: no*
*Value: A list of environment variable of the following form: KEY="val"KEY2="val2"*

> Child process spawned with taskmaster environment + the variable from `env` line in configuration file, or only taskmaster environment if `env` line is not provided.



## Shell commands



## Building and running the project

1. Download/Clone this repo

        git clone https://github.com/llefranc/42_ft_ping

2. `cd` into the root directory and run `make`

        cd 42_ft_ping
        make

3. Run `ft_ping` with appropriate permissions

		sudo ./ft_ping 192.168.0.1
		sudo ./ft_ping localhost
		sudo ./ft_ping google.com -v

## Sources

- [Documentation of supervisor][2]
- [RFC of IP protocol][3]

[1]: https://github.com/llefranc/42_taskmaster/blob/main/taskmaster.en.subject.pdf
[1]: http://supervisord.org/configuration.html
