# taskmaster (@42Paris)

> This project was code for Linux. Here is the [subject][1].
>

![Alt text](https://github.com/llefranc/42_taskmaster/blob/main/taskmaster_example.png)

## About

Taskaster is an implementation in C++ off a job control daemon, with features similar to supervisor.  

Taskmaster take as **first argument a path to a configuration file** that will be loaded at the beginning of the program. See the configuration file section for more information.

Taskmaster as **second argument a path to a log file**, where all the events will be report. If the log file specified by the path does not exist, taskmaster will create it.

![Alt text](https://github.com/llefranc/42_taskmaster/blob/main/taskmaster_log_example.png)

### Process states

A process controlled by taskmaster will be **in one of the below states** at any given time:

- :arrow_right: `STOPPED`: the process has been stopped due to a stop request or has never been started.
- :arrow_right: `STARTING`: the process is starting due to a start request.
- :arrow_right: `RUNNING`: the process is running.
- :arrow_right: `BACKOFF`: the process entered the `STARTING` state but subsequently exited too quickly (before the time defined in `starttime`) to move to the `RUNNING` state.
- :arrow_right: `STOPPING`: the process is stopping due to a stop request.
- :arrow_right: `EXITED`: The process exited from the `RUNNING` state (expectedly or unexpectedly).
- :arrow_right: `FATAL`: The process could not be started successfully.

Each process run under supervisor progresses through these states as per the following directed graph:

![Alt text](https://github.com/llefranc/42_taskmaster/blob/main/taskmaster_process_states_example.png)

## Building and running the project

1. Download/Clone this repo

        git clone https://github.com/llefranc/42_taskmaster

2. `cd` into the root directory and run `make`

        cd 42_taskmaster
        make
3. Create and fill a configuration file.

4. Run `taskmaster` by specifying the configuration file and a log file.

		./taskmaster conf/example_config_file.conf logfile

## Shell commands

When launched, taskmaster **provides to the user a shell to manage the processes**. The following commands are available:  

- `help` -- display help.
- `status` -- print all processes status.
- `start [process_name]` -- start a process.
- `stop [process_name]` -- stop a process.
- `restart [process_name]` -- restart a process (or start if not running).
- `reload` -- reload the configuration file.
- `exit` -- stop all running processes and exit.


## Understanding the configuration file

![Alt text](https://github.com/llefranc/42_taskmaster/blob/main/taskmaster_config_file_example.png)

Taskmaster need a **configuration file that will be loaded at the beginning of the program**, which indicate how to manage the different processes.  

It works in a similar way than supervisor configuration file: 

- :arrow_right: The configuration file contains one or many program blocks. 
- :arrow_right: Each program block defines the parameters for a group of process. 
- :arrow_right: Each program block are separated from others by empty lines.

The **configuration file can be reloaded** during the execution of taskmaster using the `reload` shell command. When a reload occur:  
- For the new program blocks, the corresponding processes will be added to the job control.
- For the deleted program blocks, the corresponding processes will be stopped and removed from the job control.
- For the modified program blocks, the corresponding processes will be stopped, updated and restarted if needed.
- For the unchanged program blocks, no action will be take on the corresponding processes.

> Only `[programblockname]` and `cmd` lines are mandatory in the configuration file, the others lines are optionnal and will take default values if not provided. If there is several time the same line inside a programblock, then only the last line will be taken in account.

**It will allow you to setup the following parameters for each process :**

##### 🔸`[programblockname]`
The name use to identify the group of process. This line indicate the start of a new program block and must be separated from the previous program block by a newline.  
The `programblockname` must be unique and surrounded by brackets.

*Default: none*  
*Required: yes*  
*Value: minimum 1 character, maximum 20 characters*  

##### 🔸`cmd`
The absolute path to the executable to launch and the arguments if needed.  
A non-correct path will not raise an error during the parsing, but the process launch will fail and the process will be put in a `FATAL` state (spawn failed).  

*Default: none*  
*Required: yes*  
*Value: minimum 1 character, maximum 1024*  

> **Warning: only absolute path, no relative path**  

##### 🔸 `numprocs`
The number of instances to start for a process. If superior to 1, the processes will be name the following way: "programblockname_x", where x is the instance number of the process.  

*Default: 1*  
*Required: no*  
*Value: an integer between 1 and 100*  

##### 🔸 `umask`  
An octal number (e.g. 002, 022) representing the umask of the process.  

*Default: 022*  
*Required: no*  
*Value: octal value from 000 to 777*  

##### 🔸 `workingdir`  
A file path representing the working directory of the process.  
A non-correct path will not raise an error during the parsing, but the process launch will fail and the process will be put in a `FATAL` state (spawn failed).  

*Default: working directory of taskmaster*  
*Required: no*  
*Value: minimum 1 character, maximum 1024*  

> **Warning: only absolute path, no relative path**  

##### 🔸 `autostart`  
If true, this program will start automatically when taskmaster is started.  

*Default: false*  
*Required: no*  
*Value: false, true*  

##### 🔸 `autorestart`  
Specifies if taskmaster should automatically restart a process if it exits when it is in the `RUNNING` state. If `false`, the process will not be autorestarted. If `unexpected`, the process will be restarted when the program exits with an exit code that is not one of the exit codes associated with this process’ configuration (see `exitcodes`). If `true`, the process will be unconditionally restarted when it exits, without regard to its exit code.  

*Default: false*  
*Required: no*  
*Value: false, unexpected, true*  

##### 🔸 `startretries`  
The number of serial failure attempts that taskmaster will allow when attempting to start the program before giving up and putting the process into an `FATAL` state.  

*Default: 0*  
*Required: no*  
*Value: an integer between 0 and 100*  

##### 🔸 `starttime`  
The total number of seconds which the program needs to stay running after a startup to consider the start successful (moving the process from the `STARTING` state to the `RUNNING` state). Set to 0 to indicate that the program needn’t stay running for any particular amount of time.  

*Default: 0*  
*Required: no*  
*Value: an integer between 0 and 86400*  

> Even if a process exits with an “expected” exit code (see `exitcodes`), the start will still be considered a failure if the process exits quicker than `starttime`.  

##### 🔸 `exitcodes`  
The list of “expected” exit codes for this program used with `autorestart`. If the `autorestart` parameter is set to unexpected, and the process exits in any other way than as a result of a taskmaster stop request, taskmaster will restart the process if it exits with an exit code that is not defined in this list. 

*Default: 0*  
*Required: no*  
*Value: a list of integers between 0 and 255. Each exit code must be separated by `,` character with no spaces  

##### 🔸 `stopsignal`  
The signal used to kill the program when a stop is requested. This can be specified using the signal’s name.  

*Default: TERM*  
*Required: no*  
*Value: INT, QUIT, KILL, USR1, USR2 and TERM*  

##### 🔸 `stoptime`  
The number of seconds to wait for the OS to return a SIGCHLD to taskmaster after the program has been sent a `stopsignal`. If this number of seconds elapses before taskmaster receives a SIGCHLD from the process, taskmaster will attempt to kill it with a final SIGKILL.  

*Default: 0*   
*Required: no*  
*Value: an integer between 0 and 86400*  

##### 🔸 `stdout`  
Put process stdout output in this file.  
A non-correct path will not raise an error during the parsing, but the process launch will fail and the process will be put in a `FATAL` state (spawn failed).  

*Default: *programblockname_pid*_stdout.txt*  
*Required: no*  
*Value: minimum 1 character, maximum 1024*  
> **Warning: only absolute path, no relative path**   

##### 🔸 `stderr`  
Put process stderr output in this file.  
A non-correct path will not raise an error during the parsing, but the process launch will fail and the process will be put in a `FATAL` state (spawn failed).  

*Default: *programblockname_pid*_stderr.txt*  
*Required: no*  
*Value: minimum 1 character, maximum 1024*  
> **Warning: only absolute path, no relative path**  

##### 🔸 `env`  
A list of key/value pairs in the form `KEY="val",KEY2="val2"` that will be placed in the child process’ environment. Each environment variable must be separated by `,` character without any spaces around, and the value of the environment variable must be between double quotes.  

*Default: empty list*  
*Required: no*  
*Value: A list of environment variable of the following form: `KEY="val",KEY2="val2"`*  

> Child process spawned with taskmaster environment + the variable from `env` line in configuration file, or only taskmaster environment if `env` line is not provided.

## Sources

- [Documentation of supervisor][2]

[1]: https://github.com/llefranc/42_taskmaster/blob/main/taskmaster.en.subject.pdf
[2]: http://supervisord.org/configuration.html
