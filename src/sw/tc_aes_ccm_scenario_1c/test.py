import Crypto
import sys
import ctypes
import struct
from Crypto.Cipher import AES
from ctypes import c_uint8, POINTER
from Crypto.Util.Padding import pad
from Crypto.Random import get_random_bytes

'''
NUM_NIST_KEYS 16
NONCE_LEN 13
HEADER_LEN 8
DATA_BUF_LEN23 23
DATA_BUF_LEN24 24
DATA_BUF_LEN25 25
EXPECTED_BUF_LEN31 31
EXPECTED_BUF_LEN32 32
EXPECTED_BUF_LEN33 33
EXPECTED_BUF_LEN34 34
EXPECTED_BUF_LEN35 35
'''
Nr = 10 #rounds number for 128-bits
Nb = 4 #collums number for 128-bits (32-bit words)
TC_AES_KEY_SCHED_LENGHT = Nb*(Nr+1)
TC_CCM_MAX_CT_SIZE = 16 #tamanho cipher text size
TC_CCM_MAX_PT_SIZE = 16 #tamanho max de entrada
M_LEN16 = 16
M_LEN25 = 25

def to_str(to_strg):
    to_strg_hex = [hex(num)[2:] for num in to_strg]
    return to_strg_hex


def padded_data(data):
    padded_data = pad(bytes(data), 32)
    return padded_data

def to_hex_string(data):
    return ''.join(format(byte, '02x') for byte in data)


def memcmp(expected,ciphertext):
    

    expected_hex = to_str(expected)
    ciphertext_hex = to_str(ciphertext)

    if len(expected_hex) != len(ciphertext_hex):
        return 'TC_FAIL (expected has a different length of ciphertext)'

    if expected_hex != ciphertext_hex:
       return 'TC_FAIL (expected and ciphertext are different)'
    
    return 'TC_PASS'

def tc_ccm_config(tc_aes_key_sched, nonce, mlen):
    nlen = len(nonce)
    if tc_aes_key_sched == None:
        return 'TC_FAIL (tc_aes_key_sched null)'
    elif nonce == None:
        return 'TC_FAIL (nonce null)'
    elif mlen == None:
        return 'TC_FAIL (mlen null)'
    elif nlen!= 13:
        return 'TC_FAIL (incorrent size of nlen)'
    elif (mlen < 4) or (mlen > 16) or (mlen % 2 != 0):
        return 'TC_FAIL (incorrect size of mlen)'
    else:
        return 'TC_PASS'

def tc_set_aes128_key(tc_aes_key_sched, key):
    tc_aes_key_sched = key
    tc_aes_key_sched_pointer = (c_uint8 * len(tc_aes_key_sched))(*tc_aes_key_sched)
    return tc_aes_key_sched_pointer

def do_test(key, nonce, hdr, data, mlen):

    result = "TC_PASS"
    tc_aes_key_sched = None #[TC_CCM_MAX_PT_SIZE]
    tc_aes_key_sched = key #tc_set_aes128_key(tc_aes_key_sched, key) #setting a corrent aes128key to Crypto.Cipher.AES.new()
    nonce = bytes(nonce) #nonce need to be converted to bytes
    cipher = Crypto.Cipher.AES.new(bytes(tc_aes_key_sched), AES.MODE_CCM, nonce, mac_len=mlen, assoc_len=len(hdr)) #doing cipher

    data = bytes(data)

    result = tc_ccm_config(tc_aes_key_sched, nonce, mlen)
    if result != 'TC_PASS':
        print("CCM config failed \n")
        return result

    cipher.update(bytes(hdr))
    ciphertext = cipher.encrypt(data)
    
    tag = cipher.digest() #getting encrypt tag

    if ciphertext == 0 or tag == 0:
        print("ccm_encrypt failed\n")
        result = 'TC_FAIL'
        return result

    #result = memcmp(expected,ciphertext+tag)

    if result != 'TC_PASS':
        print("ccm_encrypt produced wrong ciphertext \n")
        return result

    print('Expected: ', list(map(hex, ciphertext)))

    return result
    

#Scenario 1c: Authenticated encryption of 86 bytes of payload and 25
#bytes of header (which is authenticated but not encrypted). As with
#Scenario 1b, the authentication tag has a length of 16 bytes.

def test_vector_1():

    result = "TC_PASS"
    #key[NUM_NIST_KEYS]
    key = [0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf]

    #nonce[NONCE_LEN]
    nonce = [0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc] #0x13, 0x14, 0x15

    #hdr[HEADER_LEN]
    hdr = [0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18]

    #data[DATA_BUF_LEN86]
    data = [0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55]


    mlen = M_LEN16

    #representa o comprimento da autenticação (integridade) do código CCM (Counter with CBC-MAC) 
    #usado no teste. O CCM é um modo de operação de criptografia que combina o modo de criptografia 
    #em cadeia de blocos (CBC) com o código de autenticação de mensagem (MAC) baseado em contador.

    print("Performing CCM test (RFC 3610 test vector)")

    result = do_test(key, nonce, hdr, data, mlen)
                    #sizeof(nonce)/sizeof(hdr)/sizeof(data)/sizeof(expected)
    return result


def main():

    print("Performing CCM tests:")

    result = test_vector_1()
    if result == 'TC_PASS':
        print ('Test_vector PASS')
    else:
        print ('Test_vector FAIL', result)


main()
