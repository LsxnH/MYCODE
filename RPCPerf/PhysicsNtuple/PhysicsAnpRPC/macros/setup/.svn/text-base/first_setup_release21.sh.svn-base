
run_setup()
{
    sfile="PhysicsNtuple/PhysicsAnpRPC/macros/setup/setup_atlas_production_release21.sh"

    if [ ! -f "$sfile" ]
    then
	echo "Setup script does not exist: $sfile"
	return 1
    fi

    mkdir -p ../build

    ln -s $sfile .

    source $sfile

    pushd "$TestArea/../build"

    cmake $TestArea

    cmake --build $TestArea/../build

    popd
}

(
    run_setup
)