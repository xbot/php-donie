/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_donie.h"

/* If you declare any globals in php_donie.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(donie)
*/
ZEND_DECLARE_MODULE_GLOBALS(donie)

/* True global resources - no need for thread safety here */
static int le_donie;
static int le_donie_file_descriptor;
static int le_donie_file_descriptor_persist;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("donie.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_donie_globals, donie_globals)
    STD_PHP_INI_ENTRY("donie.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_donie_globals, donie_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_donie_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_donie_compiled)
{
	char *arg = NULL;
	size_t arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "donie", arg);

	RETVAL_STRINGL(strg, len, 0);
	efree(strg);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_donie_init_globals
 */
/* Uncomment this function if you have INI entries
*/
static void php_donie_init_globals(zend_donie_globals *donie_globals)
{
	donie_globals->global_long = 2015;
	donie_globals->global_string = "Long live Donie Leigh !";
}
static void php_donie_globals_dtor(zend_donie_globals *donie_globals)
{
	php_printf("php_donie_globals_dtor triggered.");
}
/* }}} */

/* {{{ Classes
 */
/*
 * this pointer should be put into the header file,
 * so other modules can access this class.
 */
zend_class_entry *c_leigh;

/* just a simple method. */
PHP_METHOD(Leigh, helloWorld)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    php_printf("Hello World !\n");
}

/* getting handle of this object. */
PHP_METHOD(Leigh, getObjectHandle)
{
	zval *obj;

	if (zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	obj = getThis();

	RETURN_LONG(Z_OBJ_HANDLE_P(obj));
}

/* get value of the property 'bloodType' */
PHP_METHOD(Leigh, getBloodType)
{
	zval *obj, *blood_type;

	if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

	obj = getThis();

    blood_type = zend_read_property(c_leigh, obj, "bloodType", sizeof("bloodType") - 1, 1 TSRMLS_CC);

    RETURN_ZVAL(blood_type, 1, 0);
}

/* set value of the property 'bloodType' */
PHP_METHOD(Leigh, setBloodType)
{
	zval *obj, *new_value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &new_value) == FAILURE)
	{
		return;
	}

	obj = getThis();

	zend_update_property(c_leigh, obj, "bloodType", sizeof("bloodType")-1, new_value TSRMLS_CC);
}

const zend_function_entry leigh_functions[] = {
	PHP_ME(Leigh, helloWorld, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Leigh, getObjectHandle, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Leigh, getBloodType, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Leigh, setBloodType, NULL, ZEND_ACC_PUBLIC)
	/* PHP_ABSTRACT_ME(Leigh, abstractMethod, NULL)      // abstract method */
	PHP_FE_END
};

/*
 * create a new class inheriting Leigh
 */
zend_class_entry *c_hero;

/*
 * create an interface
 */
zend_class_entry *i_superman;

const zend_function_entry superman_functions[] = {
	PHP_ABSTRACT_ME(ISuperman, saveEarth, NULL)
	PHP_FE_END
};
/* }}} */

/* {{{ c functions
 */
/* file descriptor destructor */
static void php_donie_file_descriptor_dtor(zend_rsrc_list_entry *rsrc TSRMLS_CC)
{
	FILE *fp = (FILE*)rsrc->ptr;
	fclose(fp);
}

static zend_bool php_donie_autoglobal_callback(const char *name, uint name_len TSRMLS_DC)
{
	zval *donie_val;
	MAKE_STD_ZVAL(donie_val);
	array_init(donie_val);
	add_next_index_string(donie_val, "Hello autoglobals !", 1);
	ZEND_SET_SYMBOL(&EG(symbol_table), "_DONIE", donie_val);
	return 0;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(donie)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/

	time_of_minit = time(NULL);

	/* register a class */
	zend_class_entry tmp_leigh;
	INIT_CLASS_ENTRY(tmp_leigh, "Leigh", leigh_functions);
	c_leigh = zend_register_internal_class(&tmp_leigh TSRMLS_CC);

	/* declare a property initialized as null */
	zend_declare_property_null(c_leigh, "bloodType", sizeof("bloodType")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	/* declare a class constant */
	zend_declare_class_constant_double(c_leigh, "PI", sizeof("PI")-1, 3.1415926 TSRMLS_CC);

	/* declare an interface */
	zend_class_entry tmp_superman;
	INIT_CLASS_ENTRY(tmp_superman, "ISuperman", superman_functions);
	i_superman = zend_register_internal_interface(&tmp_superman TSRMLS_CC);

	/* inherit a class and implement an interface*/
	zend_class_entry tmp_hero;
	INIT_CLASS_ENTRY(tmp_hero, "Hero", NULL);
	c_hero = zend_register_internal_class_ex(&tmp_hero, c_leigh, NULL TSRMLS_CC);
	zend_class_implements(c_hero TSRMLS_CC, 1, i_superman);

	/* create a new resource type */
	le_donie_file_descriptor = zend_register_list_destructors_ex(
		php_donie_file_descriptor_dtor, NULL, PHP_DONIE_RES_NAME_FILE, module_number
	);
	/* create a persistent resource type */
	le_donie_file_descriptor_persist = zend_register_list_destructors_ex(
		NULL, php_donie_file_descriptor_dtor, PHP_DONIE_RES_NAME_FILE, module_number
	);

	/* define a persistent constant */
	REGISTER_STRING_CONSTANT("PHP_DONIE_VERSION", PHP_DONIE_VERSION, CONST_PERSISTENT | CONST_CS);

	/* init extension globals */
	ZEND_INIT_MODULE_GLOBALS(donie, php_donie_init_globals, php_donie_globals_dtor);

	/* declare userspace super globals */
	zend_register_auto_global("_DONIE", sizeof("_DONIE")-1, 0, php_donie_autoglobal_callback TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(donie)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/

	/*
	 * FILE *fp = fopen("/tmp/php-donie.txt", "a+");
	 * fprintf(fp, "php-donie shutting down on module at %d\n", time(NULL));
	 * fclose(fp);
	 */

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(donie)
{
#if defined(COMPILE_DL_DONIE) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	time_of_rinit = time(NULL);

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(donie)
{
	// Will raise segfault on fastcgi requests.
	/* FILE *fp = fopen("/tmp/php-donie.txt", "a+"); */
	/* fprintf(fp, "php-donie shutting down on module at %d\n", time(NULL)); */
	/* fclose(fp); */

	/* free(time_of_rinit); */
	/* time_of_rinit = NULL; */

#ifndef ZTS
	php_donie_globals_dtor(&donie_globals);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(donie)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "donie support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ donie_functions[]
 *
 * Every user visible function must have an entry in donie_functions[].
 */

/* yet another hello-world. */
ZEND_FUNCTION(donie_hello)
{
	php_printf("Hello donie !");
}

/* module and request resouce usage demo. */
ZEND_FUNCTION(donie_test_resource)
{
	php_printf("time_of_minit: %d<br/>", time_of_minit);
	php_printf("time_of_rinit: %d<br/>", time_of_rinit);
	return;
}

/* return string demo & check if return value is used. */
ZEND_FUNCTION(donie_get_name)
{
	if (return_value_used)
	{
		RETURN_STRING("Donie Leigh", 1);
	}
	else
	{
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "You should use my return value.");
        RETURN_NULL();
	}
}

/*
 * Test parsing function parameters.
 */
ZEND_FUNCTION(donie_parse_parameters)
{
	char *name;
	int name_len;
	int age;
	char *optional_name = "leigh";
	int optional_name_len = sizeof(optional_name) - 1;
	zval *nonsense;
	zval *forced_seperation;
	zval *unforced_seperation;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "slz!z/z|s", &name, &name_len, &age, &nonsense, &forced_seperation, &unforced_seperation, &optional_name, &optional_name_len) == FAILURE)
	{
		RETURN_NULL();
	}

	php_printf("Got string param: name=%s, length is %d\n", name, name_len);
	php_printf("Got int param: age=%d\n", age);

	if (!nonsense)
	{
		php_printf("Got a C-NULL for param $nonsense.\n");
	}
	else
	{
		php_printf("Got some value for nonsense.\n");
	}

	php_printf("forced_seperation->refcount__gc=%d\n", forced_seperation->refcount__gc);
	php_printf("unforced_seperation->refcount__gc=%d\n", unforced_seperation->refcount__gc);

	php_printf("Optional param: optional_name=%s, length is %d\n", optional_name, optional_name_len);
}

/*
 * test hashtable operations
 */

static int hashtable_traverse_callback(void *pDest TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval **zv = (zval **) pDest;
	char *arg1 = va_arg(args, char*);

	php_printf("The first argument is %s.\n", arg1);

	if (hash_key->nKeyLength == 0)
	{
		php_printf("K-V: %d=>%Z\n", hash_key->h, *zv);
	}
	else
	{
		php_printf("K-V: %s=>%Z\n", hash_key->arKey, *zv);
	}

	return ZEND_HASH_APPLY_KEEP;
}

ZEND_FUNCTION(donie_test_hashtable)
{
	// init hashtable
	HashTable *myht;
	ALLOC_HASHTABLE(myht);
	if (zend_hash_init(myht, 100, NULL, NULL, 0) == FAILURE)
	{
		FREE_HASHTABLE(myht);
		RETURN_NULL();
	}

	// add a string with an integer key 2 to myht
	zval *zv1;
	MAKE_STD_ZVAL(zv1);
	ZVAL_STRING(zv1, "Hello HT !", 1);
	zend_hash_index_update(myht, 2, &zv1, sizeof(zval *), NULL);

	// get the next free key
	php_printf("The next free key will be %ld.\n", zend_hash_next_free_element(myht));

	// append an integer to myht
	zval *zv2;
	MAKE_STD_ZVAL(zv2);
	ZVAL_LONG(zv2, 2015);
	if (zend_hash_next_index_insert(myht, &zv2, sizeof(zval *), NULL) == FAILURE)
	{
		php_printf("HashTable appendation failed.\n");
	}
	else
	{
		php_printf("HashTable appendation succeeded.\n");
	}

	// get the size
	php_printf("HashTable has a size of %d.\n", zend_hash_num_elements(myht));

	// check if an integer key exists
	int idx = 3;
	if (zend_hash_index_exists(myht, idx))
	{
		php_printf("HashTable has an index of the value %ld.\n", idx);
	}
	else
	{
		php_printf("HashTable does not have an index of the value %ld.\n", idx);
	}

	// get a value by its key
	zval **zval_dest;
	if (zend_hash_index_find(myht, idx, (void **) &zval_dest) == SUCCESS)
	{
		php_printf("The value indexed by %ld is %Z.\n", idx, *zval_dest);
	}
	else
	{
		php_printf("The value indexed by %ld does not exist.\n", idx);
	}

	// delete the specified value from myht
	if (zend_hash_index_del(myht, idx) == FAILURE)
	{
		php_printf("The value indexed by %ld failed to be deleted.\n", idx);
	}
	else
	{
		php_printf("The value indexed by %ld is deleted.\n", idx);
	}

	// add an integer indexed by a string key, using zend_hash_update()
	zval *zv3;
	MAKE_STD_ZVAL(zv3);
	ZVAL_LONG(zv3, 1985);
	zend_hash_update(myht, "year", sizeof("year"), &zv3, sizeof(zval *), NULL);
	php_printf("An integer is updated to the hash-table indexed by a string key.\n");

	// add a string indexed by a string key, using zend_hash_add()
	zval *zv4;
	MAKE_STD_ZVAL(zv4);
	ZVAL_STRING(zv4, "Great Donie !", 1);
	if (zend_hash_add(myht, "motto", sizeof("motto"), &zv4, sizeof(zval *), NULL) == FAILURE)
	{
		php_printf("Cannot add a string indexed by a string key to the hash-table, may be the index already exists.\n");
	}
	else
	{
		php_printf("A string is added to the hash-table indexed by a string key.\n");
	}

	// get the next free key
	php_printf("The next free key will be %ld.\n", zend_hash_next_free_element(myht));

	// check if a string key exists
	char *key1 = "year";
	if (zend_hash_exists(myht, key1, strlen(key1)+1))
	{
		php_printf("The key %s exists.\n", key1);
	}
	else
	{
		php_printf("The key %s does not exist.\n", key1);
	}

	// get the value indexed by a string key
	zval **zv_dest2;
	if (zend_hash_find(myht, key1, strlen(key1)+1, (void **) &zv_dest2) == SUCCESS)
	{
		php_printf("The value indexed by %s is %Z.\n", key1, *zv_dest2);
	}
	else
	{
		php_printf("Failed fetching the value indexed by %s.\n", key1);
	}

	// delete the value indexed by a string key
	if (zend_hash_del(myht, key1, strlen(key1)+1) == SUCCESS)
	{
		php_printf("The value indexed by %s is deleted.\n", key1);
	}
	else
	{
		php_printf("The value indexed by %s failed to be deleted.\n", key1);
	}

	// quick operations leveraging a one-time hashed value
	zval *zv5;
	MAKE_STD_ZVAL(zv5);
	ZVAL_STRING(zv5, "Great Donie Leigh !", 1);

	ulong h;
	h = zend_get_hash_value("motto", sizeof("motto"));
	zend_hash_quick_update(myht, "motto", sizeof("motto"), h, &zv5, sizeof(zval *), NULL);
	php_printf("The value indexed by motto is updated with the quick operation.\n");

	// traverse the hash table.
	php_printf("Begin traversing the hash table:\n");
	zend_hash_apply_with_arguments(myht, hashtable_traverse_callback, 1, "nonsense");

	// iterating the hash table
	php_printf("Begin iterating the hash table:\n");
	HashPosition pos;
	zval **data;
	char *str_idx;
	uint str_len;
	ulong num_idx;
	for (zend_hash_internal_pointer_reset_ex(myht, &pos);
		zend_hash_get_current_data_ex(myht, (void **) &data, &pos) == SUCCESS;
		zend_hash_move_forward_ex(myht, &pos)
	) {
		switch (zend_hash_get_current_key_ex(myht, &str_idx, &str_len, &num_idx, 0, &pos)) {
			case HASH_KEY_IS_LONG:
				php_printf("K-V: %d=>%Z\n", num_idx, *data);
				break;
			case HASH_KEY_IS_STRING:
				php_printf("K-V: %s=>%Z\n", str_idx, *data);
				break;
		}
	}

	// destroy hashtable
	zend_hash_destroy(myht);
	FREE_HASHTABLE(myht);

	RETURN_NULL();
}

/* test array operations */
PHP_FUNCTION(donie_get_arr)
{
	array_init(return_value);

	// add an integer to the given position
	add_index_long(return_value, 1, 2015);

	// append a string to the array
	add_next_index_string(return_value, "dummy string", 1);

	// add a boolean value to the given key
	add_assoc_bool(return_value, "rightOrWrong", 0);

	// take care of string lengths
	add_assoc_stringl_ex(return_value, "keyStringL\0", sizeof("keyStringL\0")-1, "valueEx\0", sizeof("valueEx\0"), 1);

	// store an object in the array
	zval *obj;
	MAKE_STD_ZVAL(obj);
	object_init(obj);
	add_next_index_zval(return_value, obj);
}

PHP_FUNCTION(donie_fopen)
{
	FILE *fp;
	char *filename, *mode;
	int filename_len, mode_len;
	zend_bool persist = 0;
	char *hash_key;
	int hash_key_len;
	list_entry *persist_file;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|b", &filename, &filename_len, &mode, &mode_len, &persist) == FAILURE)
	{
		RETURN_NULL();
	}
	if (!filename_len || !mode_len)
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid file name or mode.");
		RETURN_FALSE;
	}

	/* reuse persistent resource if exists */
	hash_key_len = spprintf(&hash_key, 0, "php_donie_file_descriptor:%s-%s", filename, mode);
	if (zend_hash_find(&EG(persistent_list), hash_key, hash_key_len+1, (void **)&persist_file) == SUCCESS)
	{
		ZEND_REGISTER_RESOURCE(return_value, persist_file->ptr, le_donie_file_descriptor_persist);
		efree(hash_key);
		return;
	}

	fp = fopen(filename, mode);
	if (!fp)
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed opening %s with mode %s.", filename, mode);
		RETURN_FALSE;
	}

	/* this is the key point for registering resources */
	if (persist)
	{
		ZEND_REGISTER_RESOURCE(return_value, fp, le_donie_file_descriptor_persist);
		list_entry le;
		le.type = le_donie_file_descriptor_persist;
		le.ptr = fp;
		zend_hash_update(&EG(persistent_list), hash_key, hash_key_len+1, (void*)&le, sizeof(list_entry), NULL);
	}
	else
	{
		ZEND_REGISTER_RESOURCE(return_value, fp, le_donie_file_descriptor);
	}
	efree(hash_key);
}

PHP_FUNCTION(donie_fwrite)
{
	FILE *fp;
	zval *file_resource;
	char *data;
	int data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &file_resource, &data, &data_len) == FAILURE)
	{
		RETURN_NULL();
	}

	ZEND_FETCH_RESOURCE2(fp, FILE*, &file_resource, -1, PHP_DONIE_RES_NAME_FILE, le_donie_file_descriptor, le_donie_file_descriptor_persist);
	RETURN_LONG(fwrite(data, 1, data_len, fp));
}

PHP_FUNCTION(donie_fclose)
{
	FILE *fp;
	zval *file_resource;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &file_resource) == FAILURE)
	{
		RETURN_NULL();
	}

	ZEND_FETCH_RESOURCE2(fp, FILE*, &file_resource, -1, PHP_DONIE_RES_NAME_FILE, le_donie_file_descriptor, le_donie_file_descriptor_persist);
	zend_hash_index_del(&EG(regular_list), Z_RESVAL_P(file_resource));
	RETURN_TRUE;
}

/* show extension globals */
ZEND_FUNCTION(donie_test_ext_globals)
{
	php_printf("%s", DONIE_G(global_string));
}

/* register all functions here. */
const zend_function_entry donie_functions[] = {
	PHP_FE(confirm_donie_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(donie_hello,	NULL)
	ZEND_FALIAS(donie_hi, donie_hello, NULL)	/* function aliasing demo. */
	PHP_FE(donie_test_resource,	NULL)
	PHP_FE(donie_get_name,	NULL)
	PHP_FE(donie_parse_parameters,	NULL)
	PHP_FE(donie_test_hashtable,	NULL)
	PHP_FE(donie_get_arr,	NULL)
	PHP_FE(donie_test_ext_globals,	NULL)
	PHP_FE_END	/* Must be the last line in donie_functions[] */
};
/* }}} */

/* {{{ donie_module_entry
 */
zend_module_entry donie_module_entry = {
	STANDARD_MODULE_HEADER,
	"donie",
	donie_functions,
	PHP_MINIT(donie),
	PHP_MSHUTDOWN(donie),
	PHP_RINIT(donie),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(donie),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(donie),
	PHP_DONIE_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_DONIE
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(donie)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
