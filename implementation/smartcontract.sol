pragma solidity ^0.4.0;

contract owner{
   
    address public owner;
    uint[] whitelist;
    
    constructor() public{
        owner=msg.sender;
    }
     function add(uint a) public  
    {
        if(msg.sender==owner){
           whitelist.push(a); 
        }
    }
    function getlist() returns( uint[]){
        return whitelist;
    }
    function check( uint a) public  returns(uint){
        uint i;
        uint flag=0;
        for(i=0; i<whitelist.length; i++){
            if(whitelist[i] == a){
                flag=1;
                break;
            }
        }
           
        if (flag==0){
            return 0;
        }
        else{
            return 1;
        }
        
    }
}    