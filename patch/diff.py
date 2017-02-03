#!/usr/bin/env python
import os
caffe_dir='caffe'

# @brief Diff
def dir_diff(left_dir, right_dir, output_dir):
    for root, dirs, files in os.walk(right_dir):
        for f in files:
            right_file = os.path.join(root, f)
            if f.find('hpp') != -1 or f.find('cpp') != -1 or f.find('proto') != -1:
                relpath = os.path.relpath(right_file, right_dir)
                left_file = os.path.join(left_dir, relpath)
                output_file = os.path.join(output_dir, relpath) + '.patch'
                d = os.path.dirname(output_file)
                if not os.path.isdir(d):
                    os.makedirs(d)
                os.system("diff -u %s %s > %s" % (left_file, right_file, output_file))
                if os.path.getsize(output_file) == 0:
                    os.remove(output_file)

dir_diff(caffe_dir + '/include', '../include', 'include')
dir_diff(caffe_dir + '/src', '../src', 'src')
dir_diff(caffe_dir + '/tools', '../tools', 'tools')
