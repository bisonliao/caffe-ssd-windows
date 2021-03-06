#!/bin/bash

# run this script from cygwin  :)

root_dir=/cygdrive/e/DeepLearning/data/VOCdevkit/
root_dir2=e:/DeepLearning/data/VOCdevkit/

sub_dir=ImageSets/Main
bash_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo $bash_dir
bash_dir2=d:/software/caffe_install/caffe-ssd/data/VOC0712
for dataset in trainval val
do
  dst_file=$bash_dir/$dataset.txt
  if [ -f $dst_file ]
  then
    rm -f $dst_file
  fi
  for name in VOC2012
  do
    echo "Create list for $name $dataset..."
    dataset_file=$root_dir/$name/$sub_dir/$dataset.txt

    img_file=$bash_dir/$dataset"_img.txt"
    cp $dataset_file $img_file
    sed -i "s/^/$name\/JPEGImages\//g" $img_file
    sed -i "s/$/.jpg/g" $img_file

    label_file=$bash_dir/$dataset"_label.txt"
    cp $dataset_file $label_file
    sed -i "s/^/$name\/Annotations\//g" $label_file
    sed -i "s/$/.xml/g" $label_file

    paste -d' ' $img_file $label_file >> $dst_file

    rm -f $label_file
    rm -f $img_file
  done

  # Generate image name and size infomation.
  if [ $dataset == "val" ]
  then
	  echo /cygdrive/d/software/caffe_install/caffe-ssd/build/tools/Release/get_image_size 
	  echo $root_dir2 
	  echo $bash_dir2/$dataset.txt 
	  echo $bash_dir2/$dataset"_name_size.txt"
    /cygdrive/d/software/caffe_install/caffe-ssd/build/tools/Release/get_image_size $root_dir2 $bash_dir2/$dataset.txt $bash_dir2/$dataset"_name_size.txt"
  fi

  # Shuffle trainval file.
  if [ $dataset == "trainval" ]
  then
    rand_file=$dst_file.random
    cat $dst_file | perl -MList::Util=shuffle -e 'print shuffle(<STDIN>);' > $rand_file
    mv $rand_file $dst_file
  fi
done
