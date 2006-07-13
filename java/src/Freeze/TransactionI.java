// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package Freeze;

class TransactionI extends Ice.LocalObjectImpl implements Transaction
{
    public void
    commit()
    {
	String txnId = null;

	try
	{
	    _connection.closeAllIterators();

	    if(_txTrace >= 1)
	    {
		txnId = Long.toHexString((_txn.getId() & 0x7FFFFFFF) + 0x80000000L); 
	    }

	    _txn.commit();

	    if(_txTrace >= 1)
	    {
		_connection.communicator().getLogger().trace("Freeze.Map", _errorPrefix + "committed transaction " +
							     txnId);
	    }
	}
	catch(com.sleepycat.db.DeadlockException e)
	{
	    if(_txTrace >= 1)
	    {
		_connection.communicator().getLogger().trace("Freeze.Map", _errorPrefix +
							     "failed to commit transaction " + txnId + ": " +
							     e.getMessage());
	    }

	    DeadlockException ex = new DeadlockException();
	    ex.initCause(e);
	    ex.message = _errorPrefix + "DbTxn.commit: " + e.getMessage();
	    throw ex;
	}
	catch(com.sleepycat.db.DatabaseException e)
	{
	    if(_txTrace >= 1)
	    {
		_connection.communicator().getLogger().trace("Freeze.Map", _errorPrefix +
							     "failed to commit transaction " + txnId + ": " +
							     e.getMessage());
	    }

	    DatabaseException ex = new DatabaseException();
	    ex.initCause(e);
	    ex.message = _errorPrefix + "DbTxn.commit: " + e.getMessage();
	    throw ex;
	}
	finally
	{
	    _connection.clearTransaction();
	    _connection = null;
	    _txn = null;
	}
    }

    public void
    rollback()
    {
	String txnId = null;

	try
	{
	    _connection.closeAllIterators();
	 
	    if(_txTrace >= 1)
	    {
		txnId = Long.toHexString((_txn.getId() & 0x7FFFFFFF) + 0x80000000L); 
	    }

	    _txn.abort();

	    if(_txTrace >= 1)
	    {
		_connection.communicator().getLogger().trace("Freeze.Map", _errorPrefix + "rolled back transaction " +
							     txnId);
	    }
	}
	catch(com.sleepycat.db.DeadlockException e)
	{
	    if(_txTrace >= 1)
	    {
		_connection.communicator().getLogger().trace("Freeze.Map", _errorPrefix +
							     "failed to rollback transaction " + txnId + ": " +
							     e.getMessage());
	    }

	    DeadlockException ex = new DeadlockException();
	    ex.initCause(e);
	    ex.message = _errorPrefix + "DbTxn.abort: " + e.getMessage();
	    throw ex;
	}
	catch(com.sleepycat.db.DatabaseException e)
	{
	    if(_txTrace >= 1)
	    {
		_connection.communicator().getLogger().trace("Freeze.Map", _errorPrefix +
							     "failed to rollback transaction " + txnId + ": " +
							     e.getMessage());
	    }

	    DatabaseException ex = new DatabaseException();
	    ex.initCause(e);
	    ex.message = _errorPrefix + "DbTxn.abort: " + e.getMessage();
	    throw ex;
	}
	finally
	{
	    _connection.clearTransaction();
	    _connection = null;
	    _txn = null;
	}
    }
 
    TransactionI(ConnectionI connection)
    {
	_connection = connection;
	_txTrace = connection.txTrace();
	_errorPrefix = "Freeze DB DbEnv(\"" + _connection.envName() + "\"): ";

	try
	{
	    _txn = _connection.dbEnv().getEnv().beginTransaction(null, null);
	    
	    if(_txTrace >= 1)
	    {
		String txnId = Long.toHexString((_txn.getId() & 0x7FFFFFFF) + 0x80000000L); 

		_connection.communicator().getLogger().trace("Freeze.Map", _errorPrefix + "started transaction " +
							     txnId);
	    }
	}
	catch(com.sleepycat.db.DatabaseException e)
	{
	    if(_txTrace >= 1)
	    {
		_connection.communicator().getLogger().trace("Freeze.Map", _errorPrefix +
							     "failed to start transaction: " + e.getMessage());
	    }

	    DatabaseException ex = new DatabaseException();
	    ex.initCause(e);
	    ex.message = _errorPrefix + "txn_begin: " + e.getMessage();
	    throw ex;
	}
    }

    com.sleepycat.db.Transaction
    dbTxn()
    {
	return _txn;
    }

    private ConnectionI _connection;
    private int _txTrace;
    private com.sleepycat.db.Transaction _txn;

    private String _errorPrefix;
}
