-- ERC20合约
-- 支持多签合约持有本合约代币的版本

type State = 'NOT_INITED' | 'COMMON' | 'PAUSED' | 'STOPPED'

type Storage = {
    name: string,
    symbol: string,
    supply: int,
    precision: int, -- only used to display
    -- allowed: Map<string>, -- 各用户授权给spender用户可以多次提现的余额，每一项值是一个 userAddress=>amount int构成的json string
    -- lockedAmounts: Map<string>, -- userAddress => "lockedAmount,unlockBlockNumber"
    state: string,
    allowLock: bool,
    fee: int, -- 手续费
    minTransferAmount: int, -- 每次最低转账金额
    feeReceiveAddress: string, -- 手续费接收地址
    admin: string -- admin user address
}

-- events: Transfer, Paused, Resumed, Stopped, AllowedLock, Locked, Unlocked

var M = Contract<Storage>()

function M:init()
    self.storage.name = ''
    self.storage.symbol = ''
    self.storage.supply = 0
    self.storage.precision = 0
    self.storage.state = 'NOT_INITED'
    self.storage.admin = caller_address
    self.storage.allowLock = false
    self.storage.fee = 0
    self.storage.minTransferAmount = 0
    self.storage.feeReceiveAddress = caller_address
end

let function checkAdmin(self: table)
    if self.storage.admin ~= caller_address then
        return error("you are not admin, can't call this function")
    end
end

let function check_caller_frame_valid(M: table)
    let prev_contract_id =  get_prev_call_frame_contract_address()
    let prev_api_name = get_prev_call_frame_api_name()
    if (not prev_contract_id) or (#prev_contract_id < 1) then
        return true
    elseif (prev_api_name == "vote") or (prev_api_name == "voteFunc") then
        return true
    else
        return error("this api can't called by invalid contract")
    end
end

-- parse a,b,c format string to [a,b,c]
let function parse_args(arg: string, count: int, error_msg: string)
    if not arg then
        return error(error_msg)
    end
    let parsed = string.split(arg, ',')
    if (not parsed) or (#parsed ~= count) then
        return error(error_msg)
    end
    return parsed
end

let function parse_at_least_args(arg: string, count: int, error_msg: string)
    if not arg then
        return error(error_msg)
    end
    let parsed = string.split(arg, ',')
    if (not parsed) or (#parsed < count) then
        return error(error_msg)
    end
    return parsed
end

let function arrayContains(col: Array<object>, item: object)
    if not item then
        return false
    end
    var value: object
    for _, value in ipairs(col) do
        if value == item then
            return true
        end
    end
    return false
end

function M:on_deposit(amount: int)
    return error("not support deposit to token")
end

let function get_from_address()
    -- 支持合约作为代币持有者
    var from_address: string
    let prev_contract_id = get_prev_call_frame_contract_address()
    if prev_contract_id and is_valid_contract_address(prev_contract_id) then
        -- 如果来源方是合约时
        from_address = prev_contract_id
    else
        from_address = caller_address
    end
    return from_address
end

-- arg: name,symbol,supply,precision
function M:init_token(arg: string)
    checkAdmin(self)
    check_caller_frame_valid(self)
    pprint('arg:', arg)
    if self.storage.state ~= 'NOT_INITED' then
        return error("this token contract inited before")
    end
    let parsed = parse_args(arg, 4, "argument format error, need format: name,symbol,supply,precision")
    let info = {name: parsed[1], symbol: parsed[2], supply: tointeger(parsed[3]), precision: tointeger(parsed[4])}
    if not info.name then
        return error("name needed")
    end
    self.storage.name = tostring(info.name)
    if not info.symbol then
        return error("symbol needed")
    end
    self.storage.symbol = tostring(info.symbol)
    if not info.supply then
        return error("supply needed")
    end
    let supply = tointeger(info.supply)
    if (not supply) or (supply <= 0) then
        return error("supply must be positive integer")
    end
    self.storage.supply = supply

    let from_address = get_from_address()
    if from_address ~= caller_address then
        return error("init_token can't be called from other contract")
    end

    fast_map_set('users', caller_address, supply)

    if not info.precision then
        return error("precision needed")
    end
    let precision = tointeger(info.precision)
    if (not precision) or (precision <= 0) then
        return  error("precision must be positive integer")
    end
    let allowedPrecisions = [1,10,100,1000,10000,100000,1000000,10000000,100000000]
    if not (arrayContains(allowedPrecisions, precision)) then
        return error("precision can only be positive integer in " .. json.dumps(allowedPrecisions))
    end
    self.storage.precision = precision
    self.storage.state = 'COMMON'
    let supplyStr = tostring(supply)
    emit Inited(supplyStr)
end

let function checkState(self: table)
    if self.storage.state == 'NOT_INITED' then
        return error("contract token not inited")
    end
    if self.storage.state == 'PAUSED' then
        return error("contract paused")
    end
    if self.storage.state == 'STOPPED' then
        return error("contract stopped")
    end
end

let function checkStateInited(self: table)
    if self.storage.state == 'NOT_INITED' then
        return error("contract token not inited")
    end
end

let function checkAddress(addr: string)
    let result = is_valid_address(addr)
    if not result then
        return error("address format error")
    end
    return result
end

offline function M:state(arg: string)
    return self.storage.state
end

offline function M:tokenName(arg: string)
    checkStateInited(self)
    return self.storage.name
end

offline function M:precision(_: string)
    checkStateInited(self)
    return self.storage.precision
end

offline function M:tokenSymbol(arg: string)
    checkStateInited(self)
    return self.storage.symbol
end

offline function M:admin(_: string)
    checkStateInited(self)
    return self.storage.admin
end

offline function M:totalSupply(arg: string)
    checkStateInited(self)
    return self.storage.supply
end

offline function M:isAllowLock(_: string)
    let resultStr = tostring(self.storage.allowLock)
    return resultStr
end

offline function M:fee(_: string)
    let feeStr = tostring(self.storage.fee)
    return feeStr
end

offline function M:minTransferAmount(_: string)
    let minTransferAmountStr = tostring(self.storage.minTransferAmount)
    return minTransferAmountStr
end

offline function M:feeReceiveAddress(_: string)
    return self.storage.feeReceiveAddress
end

function M:setFee(feeStr: string)
    checkAdmin(self)
    checkState(self)
    let fee = tointeger(feeStr)
    if (fee ~= 0) and ((not fee) or (fee < 0)) then
        return error("error fee format")
    end
    self.storage.fee = fee
    emit FeeChanged(feeStr)
end

function M:setMinTransferAmount(minTransferAmountStr: string)
    checkAdmin(self)
    checkState(self)
    let minTransferAmount = tointeger(minTransferAmountStr)
    if (minTransferAmount ~= 0) and ((not minTransferAmount) or (minTransferAmount < 0)) then
        return error("error minTransferAmount format")
    end
    self.storage.minTransferAmount = minTransferAmount
    emit MinTransferAmountChanged(minTransferAmountStr)
end

function M:setFeeReceiveAddress(feeReceiveAddress: string)
    checkAdmin(self)
    checkState(self)
    if not is_valid_address(feeReceiveAddress) then
        return error("invalid address")
    end
    if is_valid_contract_address(feeReceiveAddress) then
        return error("can't use contract address")
    end
    self.storage.feeReceiveAddress = feeReceiveAddress
    emit FeeReceiveAddressChanged(feeReceiveAddress)
end


function M:openAllowLock(_: string)
    checkAdmin(self)
    checkState(self)
    check_caller_frame_valid(self)
    if self.storage.allowLock then
        return error("this contract had been opened allowLock before")
    end
    self.storage.allowLock = true
    emit AllowedLock("")
end

let function getBalanceOfUser(self: table, addr: string)
    return tointeger(fast_map_get('users', addr) or 0)
end

offline function M:balanceOf(owner: string)
    checkStateInited(self)
    if (not owner) or (#owner < 1) then
        return error('arg error, need owner address as argument')
    end
    checkAddress(owner)
    let amount = getBalanceOfUser(self, owner)
    let amountStr = tostring(amount)
    return amountStr
end

-- arg: limit(1-based),offset(0-based)}
offline function M:users(arg: string)
    return error("not implemented, you can find users from contract transaction history")
end

-- arg: to_address,integer_amount[,memo]
function M:transfer(arg: string)
    checkState(self)
    check_caller_frame_valid(self)
    let parsed = parse_at_least_args(arg, 2, "argument format error, need format is to_address,integer_amount[,memo]")
    let info = {to: parsed[1], amount: tointeger(parsed[2])}
    let to = tostring(info.to)
    let amount = tointeger(info.amount)
    if (not to) or (#to < 1) then
        return error("to address format error")
    end
    let fee = self.storage.fee
    let minTransferAmount = self.storage.minTransferAmount
    let feeReceiveAddress = self.storage.feeReceiveAddress
    if (not amount) or (amount < 1) then
        return error("amount format error")
    end
    if amount <= fee then
        return error("amount not enough for fee")
    end
    if amount < minTransferAmount then
        return error("only transfer amount >= " .. tostring(minTransferAmount) .. " will be accepted")
    end
    checkAddress(to)
    let from_address = get_from_address()
    var from_address_balance = tointeger(fast_map_get('users', from_address) or 0)
    if (not from_address_balance) or (from_address_balance < amount) then
        return error("you have not enoungh amount to transfer out")
    end
    if is_valid_contract_address(to) then
        -- 如果目标是合约地址，可能是多签合约，需要调用回调函数
        let multiOwnedContract = import_contract_from_address(to)
        let amountStr = tostring(amount - fee)
        if multiOwnedContract and (multiOwnedContract.on_deposit_contract_token) then
            multiOwnedContract:on_deposit_contract_token(amountStr)
        end
    end
    from_address_balance = from_address_balance - amount
    fast_map_set('users', from_address, from_address_balance)
    if from_address_balance == 0 then
        fast_map_set('users', from_address, nil)
    end
    let to_balance = tointeger(fast_map_get('users', to) or 0)
    fast_map_set('users', to, to_balance + amount - fee)
    if fee > 0 then
        fast_map_set('users', feeReceiveAddress, tointeger(fast_map_get('users', feeReceiveAddress) or 0) + fee)
    end
    let eventArgStr = json.dumps({from: from_address, to: to, amount: amount - fee, fee: fee})
    emit Transfer(eventArgStr)
end

-- spender用户从授权人授权的金额中发起转账
-- arg format: fromAddress,toAddress,amount(with precision)
function M:transferFrom(arg: string)
    checkState(self)
    check_caller_frame_valid(self)
    let parsed = parse_at_least_args(arg, 3, "argument format error, need format is fromAddress,toAddress,amount(with precision)")
    let fromAddress = tostring(parsed[1])
    let toAddress = tostring(parsed[2])
    let amount = tointeger(parsed[3])
    checkAddress(fromAddress)
    checkAddress(toAddress)
    if (not amount) or (amount <= 0) then
        return error("amount must be positive integer")
    end
    
    let fee = self.storage.fee
    let minTransferAmount = self.storage.minTransferAmount
    let feeReceiveAddress = self.storage.feeReceiveAddress
    if amount <= fee then
        return error("amount not enough for fee")
    end
    if amount < minTransferAmount then
        return error("only transfer amount >= " .. tostring(minTransferAmount) .. " will be accepted")
    end

    let from_address_balance = tointeger(fast_map_get('users', fromAddress) or 0)
    if (not from_address_balance) or (amount > from_address_balance) then
        return error("fromAddress not have enough token to withdraw")
    end
    let allowedDataStr = fast_map_get('allowed', fromAddress)
    if (not allowedDataStr) then
        return error("not enough approved amount to withdraw")
    end
    let allowedData: Map<int> = totable(json.loads(tostring(allowedDataStr)))
    let contractCaller = get_from_address()
    if (not allowedData) or (not allowedData[contractCaller]) then
        return error("not enough approved amount to withdraw")
    end
    let approvedAmount = tointeger(allowedData[contractCaller])
    if (not approvedAmount) or (amount > approvedAmount) then
        return error("not enough approved amount to withdraw")
    end
    fast_map_set('users', toAddress, tointeger(fast_map_get('users', toAddress) or 0) + amount - fee)
    if fee > 0 then
        fast_map_set('users', feeReceiveAddress, tointeger(fast_map_get('users', feeReceiveAddress) or 0) + fee)
    end
    fast_map_set('users', fromAddress, tointeger(fast_map_get('users', fromAddress) or 0) - amount)
    if tointeger(fast_map_get('users', fromAddress)) == 0 then
        fast_map_set('users', fromAddress, nil)
    end
    allowedData[contractCaller] = approvedAmount - amount
    if allowedData[contractCaller] == 0 then
        allowedData[contractCaller] = nil
    end
    fast_map_set('allowed', fromAddress, json.dumps(allowedData))
    let eventArgStr = json.dumps({from: fromAddress, to: toAddress, amount: amount - fee, fee: fee})
    emit Transfer(eventArgStr)
end

-- 授权另一个用户可以从自己的余额中提现
-- arg format: spenderAddress,amount(with precision)
function M:approve(arg: string)
    checkState(self)
    check_caller_frame_valid(self)
    let parsed = parse_at_least_args(arg, 2, "argument format error, need format is spenderAddress,amount(with precision)")
    let spender = tostring(parsed[1])
    checkAddress(spender)
    let amount = tointeger(parsed[2])
    if (not amount) or (amount < 0) then
        return error("amount must be non-negative integer")
    end
    var allowedData: Map<int>
    let contractCaller = get_from_address()
    if (not fast_map_get('allowed', contractCaller)) then
        allowedData = {}
    else
        allowedData = totable(json.loads(tostring(fast_map_get('allowed', contractCaller))))
        if not allowedData then
            return error("allowed storage data error")
        end
    end
    allowedData[spender] = amount
    fast_map_set('allowed', contractCaller, json.dumps(allowedData))
    let eventArg = {from: contractCaller, spender: spender, amount: amount}
    emit Approved(json.dumps(eventArg))
end

-- 查询一个用户被另外某个用户授权的金额
-- arg format: spenderAddress,authorizerAddress
offline function M:approvedBalanceFrom(arg: string)
    let parsed = parse_at_least_args(arg, 2, "argument format error, need format is spenderAddress,authorizerAddress")
    let spender = tostring(parsed[1])
    let authorizer = tostring(parsed[2])
    checkAddress(spender)
    checkAddress(authorizer)
    let allowedDataStr = fast_map_get('allowed', authorizer)
    if (not allowedDataStr) then
        return "0"
    end
    let allowedData: Map<int> = totable(json.loads(tostring(allowedDataStr)))
    if (not allowedData) then
        return "0"
    end
    let allowedAmount = allowedData[spender]
    if (not allowedAmount) then
        return "0"
    end
    let allowedAmountStr = tostring(allowedAmount)
    return allowedAmountStr
end

-- 查询用户授权给其他人的所有金额
-- arg format: fromAddress
offline function M:allApprovedFromUser(arg: string)
    let authorizer = arg
    checkAddress(authorizer)
    let allowedDataStr = fast_map_get('allowed', authorizer)
    if (not allowedDataStr) then
        return "{}"
    end
    return allowedDataStr
end

function M:pause(arg: string)
    check_caller_frame_valid(self)
    if self.storage.state == 'STOPPED' then
        return error("this contract stopped now, can't pause")
    end
    if self.storage.state == 'PAUSED' then
        return error("this contract paused now, can't pause")
    end
    checkAdmin(self)
    self.storage.state = 'PAUSED'
    emit Paused("")
end

function M:resume(arg: string)
    check_caller_frame_valid(self)
    if self.storage.state ~= 'PAUSED' then
        return error("this contract not paused now, can't resume")
    end
    checkAdmin(self)
    self.storage.state = 'COMMON'
    emit Resumed("")
end

function M:stop(arg: string)
    check_caller_frame_valid(self)
    if self.storage.state == 'STOPPED' then
        return error("this contract stopped now, can't stop")
    end
    if self.storage.state == 'PAUSED' then
        return error("this contract paused now, can't stop")
    end
    checkAdmin(self)
    self.storage.state = 'STOPPED'
    emit Stopped("")
end

-- arg: integer_amount,unlockBlockNumber
function M:lock(arg: string)
    checkState(self)
    check_caller_frame_valid(self)
    if (not self.storage.allowLock) then
        return error("this token contract not allow lock balance")
    end
    let parsed = parse_args(arg, 2, "arg format error, need format is integer_amount,unlockBlockNumber")
    let toLockAmount = tointeger(parsed[1])
    let unlockBlockNumber = tointeger(parsed[2])
    if (not toLockAmount) or (toLockAmount<1) then
        return error("to unlock amount must be positive integer")
    end
    if (not unlockBlockNumber) or (unlockBlockNumber < get_header_block_num()) then
        return error("to unlock block number can't be earlier than current block number " .. tostring(get_header_block_num()))
    end
    let from_address = get_from_address()
    if from_address ~= caller_address then
        return error("only common user account can lock balance") -- 只有普通账户可以锁仓，合约不能锁仓
    end
    let balance = getBalanceOfUser(self, from_address)
    if (toLockAmount > balance) then
        return error("you have not enough balance to lock")
    end
    if (not fast_map_get('lockedAmounts', from_address)) then
        fast_map_set('lockedAmounts', from_address, tostring(toLockAmount) .. ',' .. tostring(unlockBlockNumber))
    else
        return error("you have locked balance now, before lock again, you need unlock them or use other address to lock")
    end
    fast_map_set('users', from_address, balance - toLockAmount)
    emit Locked(tostring(toLockAmount))
end

function M:unlock(_: string)
    checkState(self)
    check_caller_frame_valid(self)
    if (not self.storage.allowLock) then
        return error("this token contract not allow lock balance")
    end
    let from_address = get_from_address()
    if (not fast_map_get('lockedAmounts', from_address)) then
        return error("you have not locked balance")
    end
    let lockedInfoParsed = parse_args(tostring(fast_map_get('lockedAmounts', from_address)), 2, "locked amount info format error")
    let lockedAmount = tointeger(lockedInfoParsed[1])
    let canUnlockBlockNumber = tointeger(lockedInfoParsed[2])

    if (get_header_block_num() < canUnlockBlockNumber) then
        return error("your locked balance only can be unlock after block #" .. tostring(canUnlockBlockNumber))
    end
    fast_map_set('lockedAmounts', from_address, nil)
    fast_map_set('users', from_address, getBalanceOfUser(self, from_address) + lockedAmount)
    emit Unlocked(from_address .. ',' .. tostring(lockedAmount))
end

-- arg: userAddress
-- only admin can call this api
function M:forceUnlock(arg: string)
    checkState(self)
    check_caller_frame_valid(self)
    if (not self.storage.allowLock) then
        return error("this token contract not allow lock balance")
    end
    checkAdmin(self)
    let userAddr = arg
    if (not userAddr) or (#userAddr < 1) then
        return error("argument format error, need format userAddress")
    end
    checkAddress(userAddr)

    if (not fast_map_get('lockedAmounts', userAddr)) then
        return error("this user have not locked balance")
    end
    let lockedInfoParsed = parse_args(tostring(fast_map_get('lockedAmounts', userAddr)), 2, "locked amount info format error")
    let lockedAmount = tointeger(lockedInfoParsed[1])
    let canUnlockBlockNumber = tointeger(lockedInfoParsed[2])

    if (get_header_block_num() < canUnlockBlockNumber) then
        return error("this user locked balance only can be unlock after block #" .. tostring(canUnlockBlockNumber))
    end
    fast_map_set('lockedAmounts', userAddr, nil)
    fast_map_set('users', userAddr, getBalanceOfUser(self, userAddr) + lockedAmount)
    emit Unlocked(userAddr .. ',' .. tostring(lockedAmount))
end

offline function M:lockedBalanceOf(owner: string)
    if (not fast_map_get('lockedAmounts', owner)) then
        return '0,0'
    else
        let resultStr = fast_map_get('lockedAmounts', owner)
        return resultStr
    end
end

function M:on_destroy()
    error("can't destroy token contract")
end

return M
