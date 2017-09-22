node=`uname -n`
home_3rd=~/3rd/${node}

home_boost=$home_3rd/boost
home_cppdb=$home_3rd/cppdb
home_ctp=$home_3rd/v6.3.6_20160606_api_tradeapi_linux64
home_fmt=$home_3rd/fmt


export LD_LIBRARY_PATH=$home_boost/lib:$home_cppdb/lib:$home_ctp:$home_fmt/lib


