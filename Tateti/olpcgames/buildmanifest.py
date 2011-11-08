#! /usr/bin/env python
"""Stupid little script to automate generation of MANIFEST and po/POTFILES.in

Really this should have been handled by using distutils, but oh well,
distutils is a hoary beast and I can't fault people for not wanting to 
spend days spelunking around inside it to find the solutions...
"""
from distutils.filelist import FileList
import os

def fileList( template ):
    """Produce a formatted file-list for storing in a file"""
    files = FileList()
    for line in filter(None,template.splitlines()):
        files.process_template_line( line )
    content = '\n'.join( files.files )
    return content


def main( ):
    """Do the quicky finding of files for our manifests"""
    content = fileList( open('MANIFEST.in').read() )
    open( 'MANIFEST','w').write( content )
    
    content = fileList( open('POTFILES.in').read() )
    try:
        os.makedirs( 'po' )
    except OSError, err:
        pass
    open( os.path.join('po','POTFILES.in'), 'w').write( content )

if __name__ == "__main__":
    main()
