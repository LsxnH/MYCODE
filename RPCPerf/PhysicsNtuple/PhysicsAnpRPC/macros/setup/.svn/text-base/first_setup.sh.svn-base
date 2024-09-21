
run_setup()
{
    sfile="PhysicsNtuple/PhysicsAnpRPC/macros/setup/setup_atlas_analysis_release.sh"

    if [ ! -f "$sfile" ]
    then
	echo "Setup script does not exist: $sfile"
	return 1
    fi

    ln -s PhysicsNtuple/PhysicsAnpRPC/macros/setup/setup_atlas_analysis_release.sh .

    source setup_atlas_analysis_release.sh

    pushd "PhysicsNtuple/PhysicsAnpRPC/cmt"

    cmt bro cmt config
    cmt bro make -j4

    popd
}

(
    run_setup
)