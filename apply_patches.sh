#!/bin/bash
#
# Requires: realpath, diff, patch, sed, Visual Studio MFC libraries

start_path=$PWD
cd "$(dirname "$0")"

root_path=$PWD
build_path=$PWD/build
code_path=$PWD/code
patches_path=$PWD/patches
thirdparty_path=$PWD/thirdparty

# Use find, then regex to generate the patch files against third-party code. 
patch_string=modified

for file in $(find ${code_path}/modified -type f ); do
    if [[ "$file" =~ ${patch_string} ]]; then
        base_file=$(basename "$file")

        # Determine the source file location to diff against
        orig_file=${base_file/_$patch_string/}
        src_dir=${orig_file%%_*}
        src_file=${orig_file##*_}
        patch_file=${orig_file}.patch
        patch_file=${patches_path}/$patch_file
        orig_file=

	#echo $thirdparty_path/$src_dir/$src_file $(dirname "$file")/$base_file 
        diff -Naur $thirdparty_path/$src_dir/$src_file $(dirname "$file")/$base_file > $patch_file

        # Replace the diff headers file paths such that the patch file is simpler to read
        sed -i -e "s|$(dirname "$file")/||g" ${patch_file}
        sed -i -e "s|$thirdparty_path/$src_dir/||g" ${patch_file}
    fi
done

[ -d $build_path ] || mkdir $build_path
pushd $build_path > /dev/null

patch_files=()
patch_dirs=()

# Traverse each patch, copy the files to be patched and soft-link the others
for file in $(find ${patches_path} -type f ); do
    # TOOD: Ensure it ends with .patch!
    if [[ "$file" =~ ".patch" ]]; then
        patch_files+=($file)
    fi

    base_file=$(basename "$file")
    patch_file=$patches_path/${base_file}
    target_to_patch="${base_file%_*}/${base_file##*_}"
    thirdparty_to_patch=$thirdparty_path/${target_to_patch%%/*}
    patch_target_name=${target_to_patch%%/*}
    target_to_patch=$thirdparty_path/$(echo ${target_to_patch} | sed 's/\.[^.]*$//')
    patch_target_name="$patch_target_name.mod"

    #realpath --relative-to=. $patches_path
    #realpath --relative-to=. $patch_file
    #realpath --relative-to=. $thirdparty_to_patch
    #realpath --relative-to=. $target_to_patch
    
    patchtar=${patch_file##*_}
    patchtar=${patchtar%.*}

    patch_dirs+=(${thirdparty_to_patch##*/})

    [ -d $patch_target_name ] || mkdir $patch_target_name

    # superuser.com/a/245770
    for thirdparty_code_file in $(find ${thirdparty_to_patch} -type f -regex '.*/.*\.\(c\|cpp\|h\|hpp\)$' ); do
        if [[ "$thirdparty_code_file" =~ ${patchtar} ]]; then
            [ -f ./$patch_target_name/$patchtar ] || rm $patch_target_name/$patchtar > /dev/null
            [ -f ./$patch_target_name/$patchtar ] || cp $thirdparty_code_file ./${patch_target_name}/
            chmod +w $patch_target_name/$patchtar
        else 
            [ -f ./${patch_target_name}/${thirdparty_code_file##*/} ] || ln -sf $thirdparty_code_file ./${patch_target_name}/
        fi
    done
done 


patch_dirs=( `for i in ${patch_dirs[@]}; do echo $i; done | sort -u` ) # Eliminate duplicates with sort -u. Modifies order.
#printf '%s\n' "${patch_dirs[@]}"

patch_dirs+=(thisisateststring)

for file in "${patch_files[@]}"; do
    base_file=$(basename "$file")
    # Expand the directory target array and see if it contains the directory component of the patch file
    if [[ "${patch_dirs[*]}" =~ "${base_file%_*}" ]]; then
        target_to_patch="${base_file%_*}.mod/${base_file##*_}"
        target_to_patch=$PWD/$(echo ${target_to_patch} | sed 's/\.[^.]*$//')

        # This block handles exiting and leaving behind a .reject file, it exits
        #  with an error code only on issues that aren't "Skipping patch...".
        # Using exit-on-error wasn't causing an immediate exit on test failures, it was exiting on
        #  certain errors which I didn't want to suppress, it was janky, so I just wrote this crud.
        rej=$(basename "$base_file".reject)
        if ! out=$(patch -Np1 -r "$rej" $target_to_patch $file); then
            if echo "$out" | grep -q "Reversed (or previously applied) patch detected!  Skipping patch."; then
                echo 'WARNING: Likely that one of the following hunk(s) would have FAILED!!!'
                echo '  We are skipping the reverse, ignoring the hunk (-N), and removing the reject. Discresion may be advised.'
            else 
                if [[ -s "$rej" ]]; then echo "$out" && exit 1; fi # Make sure that there are rejects.
            fi
        else
            if [[ -f "$rej" ]] && ! [[ -s "$rej" ]]; then echo "$out" && exit 1; fi # Make sure that there aren't any rejects.
        fi

        echo "$out" && echo "Removing rejected patch." && rm -f "$rej"
    fi
done

popd > /dev/null
