#!/bin/bash

function color_text()
{
    echo -e "\e[0;$2m$1\e[0m"
}

function echo_red()
{
    color_text "$1" "31"
}

## Check the path, if the last char is '/', remove the last char '/'.
function check_pathname()
{
    local sPathName=$1
    if [ -z "$sPathName" ]; then
        sPathName="/"
    elif [ "$sPathName" != "/" ]; then
        local sLength=${#sPathName}
        local sLastChar=""
        let sLength-=1
        sLastChar=${sPathName:${sLength}:1}
        ## If the last char is '/', remove the '/'.
        while [ "${sLastChar}" = "/" ];
        do
            sPathName=${sPathName:0:$sLength}
            sLength=${#sPathName}
            if [ $sLength -ge 1 ]; then
                ## If the sPathName length is greater or equal 1, remove last char.
                let sLength-=1
                sLastChar=${sPathName:$sLength:1}
            else
                ## If the sPathName length is lesser 1, the end.
                sLastChar=""
            fi
        done
    fi
    echo ${sPathName}
}

## Does the detection at the end of the path file name contain the specified extension name?
## If contain return 1, else return 0.
function has_extname()
{
    local in_file=$1
    local ext_name=$2
    #echo "[ext_name]: ${ext_name}"
    if [[ -z "$in_file" || -z "$ext_name" ]]; then
        echo 0
    elif [ "$in_file" != "/" ]; then
        local sLength=${#in_file}
        local sExtNameLen=${#ext_name}
        local sExtName=""
        #echo "sLength = $sLength"
        #echo "sExtNameLen = $sExtNameLen"
        if [ $sLength -gt $sExtNameLen ]; then
            let sLength-=$sExtNameLen
            sExtName=${in_file:$sLength:$sExtNameLen}
            #echo "sLength = $sLength"
            #echo "sExtName = $sExtName"
            if [ "$sExtName" = "$ext_name" ]; then
                echo 1
                return
            fi
        fi
    fi
    echo 0
}

##
## If filename is "*.gz" but isn't "*.tar.gz", then copy and backup it become "*.gz.sav",
## else echo the filename on the screen.
##
## If filename is "*.sav.sav" or "*.json.sav", then remove it.
##
## Otherwise echo the filename on the screen.
##
function scan_dir_impl()
{
    local work_dir
    local cur_dir
    local cur_file
    work_dir=$1
    cd "$work_dir"
    if [ "$work_dir" = "/" ]; then
        cur_dir=""
    else
        cur_dir="$(pwd)"
    fi

    for file in `ls "$cur_dir"`
    do
        if [ -d "$file" ]; then
            cd "$file"
            cur_file="${cur_dir}/${file}"
            scan_dir_impl "$cur_file"
            cd ..
        else
            cur_file="${cur_dir}/${file}"
            if [ $(has_extname "$cur_file" ".gz") -eq 1 ]; then
                if [ $(has_extname "$cur_file" ".tar.gz") -eq 0 ]; then
                    echo_red $cur_file
                    cp -f "$cur_file" "${cur_file}.sav"
                    gzip -d "$cur_file"
                else
                    echo $cur_file
                fi
            elif [[ $(has_extname "$cur_file" ".sav.sav") -eq 1 || \
                $(has_extname "$cur_file" ".json.sav") -eq 1 ]]; then
                echo_red $cur_file
                rm -f "$cur_file"
            else
                echo $cur_file
            fi
        fi
    done
}

##
## See: http://www.wenzizone.cn/?p=313
## See: http://blog.csdn.net/ztguang/article/details/51012013
##
function scan_dir()
{
    local work_dir
    work_dir=$1
    if [ -z "$work_dir" ]; then
        echo_red "\nError: The directory not specified.\n"
        echo -e "Usage: ./scan_dir.sh {directory}\n"
    elif [ -d "$work_dir" ]; then
        scan_dir_impl $work_dir
    elif [ -f "$work_dir" ]; then
        echo "work_dir = $work_dir"
        echo_red "The argument isn't a directory, it's a filename, we will trace the directory name!!"
        #work_dir=$(check_pathname "$work_dir")
        work_dir=`dirname "$work_dir"`
        echo_red "The directory name is: $work_dir"
        scan_dir_impl "$work_dir"
    else
        echo_red "The directory that you input isn't existed, please reinput and try again!!"
        exit 1
    fi
}

function uint_test()
{
    file="/usr/lib/ambari-server/web/data/abcd.json"
    value=$(has_extname "$file" ".gz")
    echo "file = $file"
    echo "value = $value"

    file="/usr/lib/ambari-server/web/data/abcd.json.sav.sav"
    value=$(has_extname "$file" ".sav.sav")
    echo "file = $file"
    echo "value = $value"
}

##
## Example commands:
##
##   scan_dir.sh /usr/lib/ambari-server/web/data
##   scan_dir.sh /usr/lib/ambari-server/web/data/alerts
##   scan_dir.sh /usr/lib/ambari-server/web/data/alerts/alert_instances_history.json.gz.sav.sav
##

set -e

# uint_test
# exit 1

in_dir=$1

# echo "in_dir = $in_dir"
# exit 1

scan_dir "$in_dir"
