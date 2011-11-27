"""Abstraction layer for working outside the Sugar environment"""
import traceback, cStringIO
import logging
log = logging.getLogger( 'olpcgames.util' )
import os
import os.path

NON_SUGAR_ROOT = '~/.sugar/default/olpcgames'

try:
    from sugar.activity.activity import get_bundle_path as _get_bundle_path
    def get_bundle_path( ):
        """Retrieve bundle path from activity with fix for silly registration bug"""
        path = _get_bundle_path()
        if path.endswith( '.activity.activity' ):
            log.warn( '''Found double .activity suffix in bundle path, truncating: %s''', path )
            path = path[:-9]
        return path
except ImportError:
    log.warn( '''Do not appear to be running under Sugar, stubbing-in get_bundle_path''' )
    def get_bundle_path():
        """Retrieve a substitute data-path for non OLPC systems"""
        return os.getcwd()


def get_activity_root( ):
    """Return the activity root for data storage operations

    If the activity is present, returns the activity's root,
    otherwise returns NON_SUGAR_ROOT as the directory.
    """
    import olpcgames
    if olpcgames.ACTIVITY:
        return olpcgames.ACTIVITY.get_activity_root()
    else:
        return os.path.expanduser( NON_SUGAR_ROOT )

def data_path(file_name):
    """Return the full path to a file in the data sub-directory of the bundle"""
    return os.path.join(get_bundle_path(), 'data', file_name)
def tmp_path(file_name):
    """Return the full path to a file in the temporary directory"""
    return os.path.join(get_activity_root(), 'tmp', file_name)

def get_traceback(error):
    """Get formatted traceback from current exception
    
    error -- Exception instance raised
    
    Attempts to produce a 10-level traceback as a string
    that you can log off.  Use like so:
    
    try:
        doSomething()
    except Exception, err:
        log.error( 
            '''Failure during doSomething with X,Y,Z parameters: %s''', 
            util.get_traceback( err ),
        )
    """
    if error is None:
        error = []
        for (f,l,func,statement) in traceback.extract_stack()[:-2]:
            if statement:
                statement = ': %s'%( statement, )
            if func:
                error.append( '%s.%s (%s)%s'%( f,func,l, statement))
            else:
                error.append( '%s (%s)%s'%( f,l, statement))
        return "\n".join( error )
    else:
        exception = str(error)
        file = cStringIO.StringIO()
        try:
            traceback.print_exc( limit=10, file = file )
            exception = file.getvalue()
        finally:
            file.close()
        return exception
