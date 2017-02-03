#!/usr/bin/env python
import os
import shutil
caffe_dir='caffe'

# @brief Patch
def dir_patch(left_dir, right_dir, diff_dir):
    for root, dirs, files in os.walk(right_dir):
        for f in files:
            right_file = os.path.join(root, f)
            if f.find('hpp') != -1 or f.find('cpp') != -1 or f.find('proto') != -1:
                relpath = os.path.relpath(right_file, right_dir)
                left_file = os.path.join(left_dir, relpath)
                diff_file = os.path.join(diff_dir, relpath) + '.patch'
                if os.path.isfile(left_file):
                    if os.path.isfile(diff_file):
                        print "patch %s %s -o %s" % (left_file, diff_file, right_file)
                        os.system("patch %s %s -o %s" % (left_file, diff_file, right_file))
                    else:
                        print 'cp %s %s' % (left_file, right_file)
                        shutil.copyfile(left_file, right_file)

dir_patch(caffe_dir + '/include', '../include', 'include')
dir_patch(caffe_dir + '/src', '../src', 'src')
dir_patch(caffe_dir + '/tools', '../tools', 'tools')
