from typing import Dict, Any, Union

DEFAULT_TIMEOUT = 500
import json
import csv
from web3 import Web3, HTTPProvider


#web3 = Web3(HTTPProvider("http://localhost:8545/",request_kwargs={'timeout':500}))
web3 = Web3(Web3.HTTPProvider(endpoint_uri='http://localhost:8545'))

web3.eth.defaultAccount= web3.eth.accounts[0]

web3.geth.personal.unlock_account(web3.eth.accounts[0], "123")
abi = json.loads('[{"constant":false,"inputs":[{"name":"a","type":"uint256"}],"name":"add","outputs":[],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":false,"inputs":[],"name":"getlist","outputs":[{"name":"","type":"uint256[]"}],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":false,"inputs":[{"name":"a","type":"uint256"}],"name":"check","outputs":[{"name":"","type":"uint256"}],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[],"name":"owner","outputs":[{"name":"","type":"address"}],"payable":false,"stateMutability":"view","type":"function"},{"inputs":[],"payable":false,"stateMutability":"nonpayable","type":"constructor"}]')


bytecode ="608060405234801561001057600080fd5b50336000806101000a81548173ffffffffffffffffffffffffffffffffffffffff021916908373ffffffffffffffffffffffffffffffffffffffff1602179055506102df806100606000396000f300608060405260043610610062576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff1680631003e2d21461006757806347dc424e146100945780635f72f450146101005780638da5cb5b14610141575b600080fd5b34801561007357600080fd5b5061009260048036038101908080359060200190929190505050610198565b005b3480156100a057600080fd5b506100a96101c7565b6040518080602001828103825283818151815260200191508051906020019060200280838360005b838110156100ec5780820151818401526020810190506100d1565b505050509050019250505060405180910390f35b34801561010c57600080fd5b5061012b6004803603810190808035906020019092919050505061021f565b6040518082815260200191505060405180910390f35b34801561014d57600080fd5b5061015661028e565b604051808273ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200191505060405180910390f35b600181908060018154018082558091505090600182039060005260206000200160009091929091909150555050565b6060600180548060200260200160405190810160405280929190818152602001828054801561021557602002820191906000526020600020905b815481526020019060010190808311610201575b5050505050905090565b6000806000809050600091505b600180549050821015610270578360018381548110151561024957fe5b906000526020600020015414156102635760019050610270565b818060010192505061022c565b60008114156102825760009250610287565b600192505b5050919050565b6000809054906101000a900473ffffffffffffffffffffffffffffffffffffffff16815600a165627a7a72305820ee5ce0c0089be35882366d8367d87811f068e4296da4ed91a86903d51cce800c0029"
deploy = web3.eth.contract(abi= abi, bytecode = bytecode)
tx_hash = deploy.constructor().transact()
print(tx_hash)
tx_receipt = web3.eth.waitForTransactionReceipt(tx_hash,500,0.1)
print(tx_receipt)

greeter = web3.eth.contract(address=tx_receipt.contractAddress, abi=abi)
with open(r'C:\\Users\vashi\Downloads\homelog.csv','rt')as f:
  data = csv.reader(f)
  i=0

  for row in data:
      if i<5:

            tx_hash=greeter.functions.add(int(row[2])).transact()
            tx_receipt = web3.eth.waitForTransactionReceipt(tx_hash)

            #print(greeter.functions.get_whitelist().call())

      i+=1
print(greeter.functions.getlist().call())
with open(r'C:\\Users\vashi\Downloads\homelog.csv','rt')as f:
  data = csv.reader(f)

  for row in data:
      print(row[2], greeter.functions.check(int(row[2])).call())