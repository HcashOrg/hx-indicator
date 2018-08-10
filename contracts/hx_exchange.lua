-- blocklink的代币兑换合约，此合约暂时只支持简单的一次性兑换，不支持部分成交

type State = 'COMMON' | 'CLOSED' 

type Storage = { 
    state: string,
    owner: string,
	sell_orders_num:int,
	owner_assets:Map<int>,
	sell_orders:Map<string>   
}

var M = Contract<Storage>()

function M:init()    
    self.storage.state = 'COMMON'
    self.storage.owner = caller_address
	self.storage.owner_assets = {}
	self.storage.sell_orders = {} 
	self.storage.sell_orders_num = 0
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

let function checkState(M: table)
    if M.storage.state ~= 'COMMON' then
        return error("state error, now state is " .. tostring(M.storage.state))
    end
end


let function check_caller_frame_valid(M: table)
    let prev_contract_id =  get_prev_call_frame_contract_address()
    let prev_api_name = get_prev_call_frame_api_name()
    if (not prev_contract_id) or (#prev_contract_id < 1) then
        return true
    else
        return error("this api can't called by invalid contract")
    end
end



let function adjustAssetAmount(assets:table,assetSymbol:string,addedAmount:int)
	var afterAmount:int = 0
	if addedAmount == 0 then
		return 
	end
	
	var orignalAmount = assets[assetSymbol]
	if (orignalAmount == nil) then
		afterAmount = addedAmount
	else
		afterAmount = tointeger(orignalAmount) + addedAmount
	end
	
	if (afterAmount < 0) then
		return error("adjust amount wrong ")
	elseif (afterAmount == 0) then
		assets[assetSymbol] = nil
	else
		assets[assetSymbol] = afterAmount
	end
end


offline function M:state(_: string)
    return self.storage.state
end

offline function M:owner(_: string)
    return self.storage.owner
end




offline function M:owner_assets(_: string)
    let owner_assets_str = json.dumps(self.storage.owner_assets)
    return owner_assets_str
end



offline function M:sell_orders_num(_: string)
    let sell_orders_num_str = json.dumps(self.storage.sell_orders_num)
    return sell_orders_num_str
end

offline function M:sell_orders(_: string)
    let sell_orders_str = json.dumps(self.storage.sell_orders)
    return sell_orders_str
end


offline function M:dumpData(_: string)
	var mm = {}
	mm["state"]=tostring(self.storage.state)
	mm["owner"]=tostring(self.storage.owner)
	mm["sell_orders_num"]=tostring(self.storage.sell_orders_num)
	mm["owner_assets"]=json.dumps(self.storage.owner_assets)
	mm["sell_orders"]=json.dumps(self.storage.sell_orders)
	let r = json.dumps(mm)
	print("storage:"..r)
    return r
end




--- 提现自己所有的币
function M:withdrawAll(_: string)
	checkState(self)
	if self.storage.owner ~= caller_address then
        return error("you have no permission to call this api")
    end
	
	var kk:string 
	var vv:int
	for kk,vv in pairs(self.storage.owner_assets) do
        if vv > 0 then
			------------transfer...
			let res1 = transfer_from_contract_to_address(caller_address, kk ,vv)
			if res1 ~= 0 then
				return error("transfer asset " .. kk .. " to " .. caller_address .. " error, error code: " .. tostring(res1))
			end
			let eventArg = caller_address .. "," ..  tostring(vv) .. "," .. kk
			print("Withdrawed:"..eventArg)
			emit Withdrawed(eventArg)	
		end
    end	
	self.storage.owner_assets = {}
	
	return "OK"
end


--- 提现  指定 币种 金额
----asset,amount  BTC,10 
function M:withdrawAsset(args:string)
	checkState(self)
	check_caller_frame_valid(self)
	if self.storage.owner ~= caller_address then
        return error("you have no permission to call this api")
    end

	var argss = parse_args(args,2,"withdrawAsset args wrong")
	var withdrawAsset = tostring(argss[1])
	var withdrawAmount = tointeger(argss[2])
	pprint(argss)
	
	if (withdrawAsset == nil) or (withdrawAmount == nil) or (withdrawAmount <= 0) then
		return error("args wrong")
	end
	
	var balance = self.storage.owner_assets[withdrawAsset]
	
	if (balance == nil) or (balance <= 0 ) then
		return error("no balance ")
	end
	
	if (balance < withdrawAmount ) then
		return error("not enough balance to withdraw ,current balance is:"..tostring(balance))
	end

	-------transfer to ...
	let res1 = transfer_from_contract_to_address(caller_address, withdrawAsset, withdrawAmount)
    if res1 ~= 0 then
        return error("transfer asset " .. withdrawAsset .. " to " .. caller_address .. " error, error code: " .. tostring(res1))
    end

	let subamount:int = 0 - withdrawAmount
	adjustAssetAmount(self.storage.owner_assets,withdrawAsset,subamount)
	
	let eventArg = caller_address .. "," ..  tostring(withdrawAmount) .. "," .. withdrawAsset
	emit Withdrawed(eventArg)
	
	print("withdraw OK")
	return "OK"
end


------ owner 撤销指定币种的所有卖单
--------args: LNK,BTC
function M:cancelSellOrderPair(args: string)
	checkState(self)
	if self.storage.owner ~= caller_address then
        return error("you have not permission to call this api")
    end

	print(args)

	let ss = parse_args(args,2,"cancelSellOrderPair args wrong")
	let sellAsset = tostring(ss[1])
	let wishBuyAsset = tostring(ss[2])


	var oriOrder = self.storage.sell_orders[sellAsset .. "," .. wishBuyAsset]
	if (oriOrder == nil) or (#oriOrder < 1)  then
		return error("sellOrder not exitst")
	end
	
	-----------------------remove sell order---------------------------------------
	print(oriOrder)
	var jsonstr = ""
	var ordermm:table = {}
	var orderArr:table = {}
	if (oriOrder == nil) or (#oriOrder < 1)  then
		return error("sellOrder not exitst")
	else
		ordermm = totable(json.loads(oriOrder))
		pprint(ordermm)
		print("orignal jsonstr:"..oriOrder)
		print(ordermm["num"])
		print(ordermm["orderArray"])
		orderArr = totable(ordermm["orderArray"])
		
		if(ordermm == nil) or (orderArr == nil) then
			return error("ori order wrong")
		end
		
		var ordernum = tointeger(ordermm["num"])
		var v = ""
		var pos = 0
		var totalSellAmount:int = 0
		var tempAmount:int 
		var ss
		for pos,v in ipairs(orderArr) do
			ss = parse_args(v,3,"sellOrder formate wrong")
			
			tempAmount = tointeger(ss[1])
			if (tempAmount == nil) or (tempAmount <= 0) then
				return error("sellOrderArray wrong")
			end
			totalSellAmount = totalSellAmount + tempAmount
		end
		
		self.storage.sell_orders[sellAsset .. "," .. wishBuyAsset] = nil
		self.storage.sell_orders_num = self.storage.sell_orders_num - ordernum
		print("totalSellAmount="..tostring(totalSellAmount))
		
		------------------modify user assets--------------------
		adjustAssetAmount(self.storage.owner_assets,sellAsset,totalSellAmount)
		
		let eventArg = sellAsset .. "," .. wishBuyAsset .. "," ..tostring(totalSellAmount)
		emit SellOrderPairCanceled(eventArg)
	end
	return "OK"
end

--- owner 撤销所有卖单
function M:cancelAllOrder(_: string)
	checkState(self)
	if self.storage.owner ~= caller_address then
        return error("you have no permission to call this api")
    end
	
	var pairArr:table = {}
	var kk:string
	var vv:string
	for kk,vv in pairs(self.storage.sell_orders) do
		table.insert(pairArr,kk)
	end
	
	var pairstr:string
	for _,pairstr in ipairs(pairArr) do
		self:cancelSellOrderPair(pairstr)
	end
	let eventArg = "cancelAllOrder"
	emit AllOrderCanceled(eventArg)
	print("cancelAllOrder over!!!")
	return "OK"
	
	
end


--- owner 关闭（会撤销所有卖单  返币给所有用户  关闭合约）
function M:close()
	checkState(self)
    if self.storage.owner ~= caller_address then
        return error("you have no permission to call this api")
    end
    
	----------------cacel all order -----------------
	self:cancelAllOrder("")
	--------------return back ------------------
	self:withdrawAll("")	
	-------------------------------------
	self.storage.state = 'CLOSED'
	print("CLOSED")
	let eventArg = "closed"
	emit Closed(eventArg)
    return "OK"
end

--- owner 提现  多余的
----asset,amount  BTC,10 
function M:withrawRemainAsset(args: string)
	if self.storage.owner ~= caller_address then
        return error("you have not permission to call this api")
    end

	if (self.storage.state ~= 'CLOSED') then
		return error("not closed ")
	end
		
	var argss = parse_args(args,2,"withdrawAsset args wrong")
	var withdrawAsset = tostring(argss[1])
	var withdrawAmount = tointeger(argss[2])
	pprint(argss)
	
	let res1 = transfer_from_contract_to_address(caller_address, withdrawAsset, withdrawAmount)
    if res1 ~= 0 then
        return error("transfer asset " .. withdrawAsset .. " to " .. caller_address .. " error, error code: " .. tostring(res1))
    end
	
	let eventArg = caller_address .. "," ..  tostring(withdrawAmount) .. "," .. withdrawAsset
	emit Withdrawed(eventArg)
	
	return "OK"
	
end

function M:on_destroy()
    return error("destroy not supported")
end


--------------------------------------------------------------
let function insertToSellArray(sellArray:table,sellAmount:int,wishBuyAmount:int )
	if (sellAmount<= 0) or (wishBuyAmount<=0) then
		return error("sell order amount wrong")
	end
	let exchangerate = wishBuyAmount/sellAmount
	print("exchangerate="..tostring(exchangerate))
	var pos = 1
	var v = ""
	var ss
	var rate = 0.0
	var amountstr = tostring(sellAmount)..","..tostring(wishBuyAmount)..","..tostring(exchangerate)
	var tableop = 0
	let EPSILON = 0.000001 
	let totalnum = #sellArray
	var findpos = 0
	
	for pos,v in ipairs(sellArray) do
		ss = parse_args(v,3,"sellOrder formate wrong")
		rate = tonumber(ss[3])
		print(rate)
		findpos = pos
		if rate == nil then
			return error("sell order rate wrong")
		end

		if ((rate - EPSILON) < exchangerate) and (exchangerate < (rate + EPSILON)) then   ----合并
			---
			var orisellAmount = tointeger(ss[1])
			var oriwishBuyAmount = tointeger(ss[2])
			if (orisellAmount== nil) or (orisellAmount <= 0) or (oriwishBuyAmount== nil) or (oriwishBuyAmount <= 0) then
				return error("sellOrderArray wrong ")
			end
			var newrate = (oriwishBuyAmount+wishBuyAmount)/(orisellAmount + sellAmount)
			amountstr = tostring(orisellAmount + sellAmount)..","..tostring(oriwishBuyAmount+wishBuyAmount)..","..tostring(newrate)
			tableop = 1
			break
		elseif rate > exchangerate then
			tableop = 2
			break
		end
	end
		
	
	if tableop == 1 then    -----modify
		print("modify:"..amountstr)
		sellArray[findpos]=amountstr
	elseif tableop == 2 then    ----- insert
		table.insert(sellArray,findpos,amountstr)
		print("insert:"..amountstr)
	else
		table.insert(sellArray,amountstr)    ----append to last
		print("append:"..amountstr)
	end
end


----------------------------------
---param: buy info  WishBuyAsset,WishBuyAssetAmount  etc: LNK,200
function M:on_deposit_asset(jsonstrArgs: string)
	print("jsonstrargs:"..jsonstrArgs)
	check_caller_frame_valid(self)

    let arg = json.loads(jsonstrArgs)
    let addedAmount = tointeger(arg.num)
    let symbol = tostring(arg.symbol)
    let param = tostring(arg.param)

	if (not addedAmount) or (addedAmount <= 0) then
		 return error("deposit should greater than 0")
	end

	if (not symbol) or (#symbol < 1) then
		 return error("on_deposit_asset arg wrong")
	end
	
	--if (extra == "") || (extra == "nil") then
	if ( caller_address == self.storage.owner ) then     ---owner deposit
		----
		checkState(self)
		adjustAssetAmount(self.storage.owner_assets,symbol,addedAmount)
		let eventarg = caller_address .. "," ..  tostring(addedAmount) .. "," .. symbol
		emit Deposited(eventarg)
		return
	end
	
	-----------------user deposit------------------------
	if (param == nil) or (param == "") or (param == "nil") then
		return error("need args to buy")
	end
	
	checkState(self)
	let argss = parse_args(param,2,"args wrong")
	let userWishBuyAsset = tostring(argss[1])
	let inputUserWishBuyAssetAmount = tointeger(argss[2])
	let userSellAsset = symbol
	let inputUserSellAssetAmount = addedAmount
	
	
	if (inputUserSellAssetAmount == nil) or (inputUserSellAssetAmount <= 0) then
		return error("inputUserSellAssetAmount should greater than 0 ")
	end
	
	----------check match or not --------------------------
	print("order pair: " ..userWishBuyAsset .. "," .. userSellAsset)
	
	var orderjsonstr = self.storage.sell_orders[userWishBuyAsset .. "," .. userSellAsset]
	
	print(orderjsonstr)
	var ordermm:table = {}
	var orderArr:table = {}
	if (orderjsonstr == nil) or (#orderjsonstr < 1) then   ---new pair
		return error("no matched order to buy ")
	end
	

	ordermm = totable(json.loads(orderjsonstr))
	pprint(ordermm)
	print("orignal jsonstr:"..orderjsonstr)
	print(ordermm["num"])
	print(ordermm["orderArray"])
	orderArr = totable(ordermm["orderArray"])

	if(ordermm == nil) or (orderArr == nil) or (#orderArr < 1) then
		return error("no sell orders ")
	end
	
	var pos = 0
	var v = ""
	var ss
	var userExchangedBuyAmount:int = 0
	var userExchangedSellAmount:int = 0
	var orderSellAmount = 0
	var orderWishBuyAmount = 0
	
	let oriOrderNum = #orderArr
	---------------buy iterate ----------------------
	var orderNum = oriOrderNum
	var userSellAssetAmount = inputUserSellAssetAmount
	var userWishBuyAssetAmount = inputUserWishBuyAssetAmount
	var buyRate = userSellAssetAmount/userWishBuyAssetAmount
	var rate:number
	let EPSILON = 0.000001 
	while (orderNum >= 1) and (userSellAssetAmount >= 1) and (userWishBuyAssetAmount >= 1) do
		ss = parse_args(tostring(orderArr[1]),3,"sellOrder formate wrong")
		rate = tonumber(ss[3])
		if rate == nil then
			return error("sell order rate wrong")
		end
		
		buyRate = userSellAssetAmount/userWishBuyAssetAmount
		print("userBuyRate="..tostring(buyRate)..",orderSellRate="..tostring(rate))

		if (rate > (buyRate + EPSILON)) then
			break
		end
		
		orderSellAmount = tointeger(ss[1])
		orderWishBuyAmount = tointeger(ss[2])
		
		if userSellAssetAmount >= orderWishBuyAmount then    -----full 
			--------------calculate --------
		
			userExchangedSellAmount = userExchangedSellAmount + orderWishBuyAmount
			userExchangedBuyAmount = userExchangedBuyAmount + orderSellAmount
			
			userSellAssetAmount = userSellAssetAmount - orderWishBuyAmount
			userWishBuyAssetAmount = userWishBuyAssetAmount - orderSellAmount
			
			table.remove(orderArr,1)
			
		else    --- partly exchange
			
		------------------------caculate ---------------
			---  userExchangedBuyAmount = userExchangedSellAmount/orderWishBuyAmount * orderSellAmount  floor  ---------------

			let calResult = userSellAssetAmount/rate
			print("calResult="..tostring(calResult))
			let tempUserExchangedBuyAmount = math.floor(calResult)
			if (tempUserExchangedBuyAmount < 1) or (userWishBuyAssetAmount > tempUserExchangedBuyAmount) then	
				print("not fill ")
				break
			end
			
			orderSellAmount = orderSellAmount - tempUserExchangedBuyAmount
			orderWishBuyAmount = orderWishBuyAmount - userSellAssetAmount
			
			if (orderSellAmount<=0) or (orderWishBuyAmount<=0) then
				break
			end
			
			userExchangedSellAmount = userExchangedSellAmount + userSellAssetAmount
			userExchangedBuyAmount = userExchangedBuyAmount + tempUserExchangedBuyAmount

			userSellAssetAmount = 0
			userWishBuyAssetAmount = 0
			
			rate = orderWishBuyAmount/orderSellAmount
			orderArr[1] = tostring(orderSellAmount)..","..tostring(orderWishBuyAmount)..","..tostring(rate)			
			break
		end
		orderNum = #orderArr

	end
	
	print("userExchangedSellAmount=" .. tostring(userExchangedSellAmount))
	print("userExchangedBuyAmount =" .. tostring(userExchangedBuyAmount))
	
	if (userExchangedBuyAmount <= 0) or (userExchangedSellAmount <= 0) then
		return error("no matched sell order to buy ")
	end
	
	---------------------change order -----------------------
	orderNum = #orderArr
	if (orderNum <= 0) then
		self.storage.sell_orders[userWishBuyAsset .. "," .. userSellAsset] = nil
	else
		ordermm["num"] = #orderArr
		ordermm["orderArray"] = orderArr
		self.storage.sell_orders[userWishBuyAsset .. "," .. userSellAsset] = json.dumps(ordermm)
	end
	self.storage.sell_orders_num = self.storage.sell_orders_num - (oriOrderNum - orderNum)
	---------------------modify owner assets--------------------
	adjustAssetAmount(self.storage.owner_assets,userSellAsset,userExchangedSellAmount)
	
	-----------------transfer----------------------------
	
	var res1 = transfer_from_contract_to_address(caller_address, userWishBuyAsset, userExchangedBuyAmount)
    if res1 ~= 0 then
        return error("transfer asset " .. userWishBuyAsset .. " to " .. caller_address .. " error, error code: " .. tostring(res1))
    end
	
	var eventArg = caller_address .. "," ..  tostring(userExchangedBuyAmount) .. "," .. userWishBuyAsset
	print("Bought:"..eventArg)
	emit Bought(eventArg)
	
	let returnAmount = inputUserSellAssetAmount - userExchangedSellAmount
	if returnAmount > 1 then
		res1 = transfer_from_contract_to_address(caller_address, userSellAsset, returnAmount)
		if res1 ~= 0 then
			return error("transfer asset " .. userSellAsset .. " to " .. caller_address .. " error, error code: " .. tostring(res1))
		end
		eventArg = caller_address .. "," ..  tostring(returnAmount) .. "," .. userSellAsset
		emit Returned(eventArg)
		print("Returned:"..eventArg)
	end
	
end



--- owner 挂卖单（owner需要先往合约存款后才能挂卖单 卖单的币会被锁定  只有撤销卖单后锁定的币才能提现）
---args:SellAsset,SellAssetAmount,WishBuyAsset,WishBuyAssetAmount    LNK,5000,BTC,9
function M:putOnSellOrder(args: string)
    check_caller_frame_valid(self)
	checkState(self)

    if self.storage.owner ~= caller_address then
        return error("you have no permission to call this api")
    end
	
	if self.storage.sell_orders_num >= 10000 then
		return error("too mush sell orders ")
	end

	print(args)
	var ss = parse_args(args,4,"putOnSellOrder args wrong")
	var sellAsset = tostring(ss[1])
	var sellAssetAmount = tointeger(ss[2])
	var wishBuyAsset = tostring(ss[3])
	var wishBuyAssetAmount = tointeger(ss[4])
	pprint(ss)
	
	if ((string.find(sellAsset," "))~= nil) or ((string.find(sellAsset," "))~= nil) then
		return error("asset has space ")
	end

	if wishBuyAsset == sellAsset then
		return error("wishBuyAsset and sellAsset should be different ")
	end
	---------------------check balance-----------------------------
	var balamount = self.storage.owner_assets[sellAsset]
	print(balamount)
	if balamount == nil then
		return error("not enough amount to sell")
	end
	
	if balamount < sellAssetAmount then
		return error("not enough amount to sell, current balance is " .. tostring(sellAsset))
	end
	
	if self.storage.sell_orders_num > 10000 then
		return error("too many sell orders")
	end
	----------------------add sell order----------------------------------------
	var oriOrder = self.storage.sell_orders[sellAsset .. "," .. wishBuyAsset]
	let EPSILON = 0.000001 
	let rate = wishBuyAssetAmount/sellAssetAmount

	if rate < (EPSILON * 2) then
		return error("too low rate ")
	end
	
	print(oriOrder)
	var jsonstr = ""
	var ordermm:table = {}
	var orderArr:table = {}
	var addOrderNum = 0
	if (oriOrder == nil) or (#oriOrder < 1) then   ---new pair
		ordermm["num"] = 1		
		orderArr[1] = tostring(sellAssetAmount) .. "," .. tostring(wishBuyAssetAmount)..","..tostring(rate)
		ordermm["orderArray"] = orderArr
		jsonstr = json.dumps(ordermm)
		print("jsonstr:"..jsonstr)
		self.storage.sell_orders[sellAsset .. "," .. wishBuyAsset] = jsonstr
		self.storage.sell_orders_num = self.storage.sell_orders_num + 1
	else
		ordermm = totable(json.loads(oriOrder))
		pprint(ordermm)
		print("orignal jsonstr:"..oriOrder)
		print(ordermm["num"])
		print(ordermm["orderArray"])
		orderArr = totable(ordermm["orderArray"])
		
		if(ordermm == nil) or (orderArr == nil) then
			return error("ori order wrong")
		end
		
		var origalOrderNum = #orderArr
		insertToSellArray(orderArr,sellAssetAmount,wishBuyAssetAmount)
		ordermm["num"] = #orderArr
		ordermm["orderArray"] = orderArr
		
		jsonstr = json.dumps(ordermm)
		self.storage.sell_orders[sellAsset .. "," .. wishBuyAsset] = jsonstr

		self.storage.sell_orders_num = self.storage.sell_orders_num + ((#orderArr) - origalOrderNum )
	end

-----------------------adjust owner balance ----------------------------------------

	var balanceAfter = balamount - sellAssetAmount 

	if balanceAfter == 0 then
		self.storage.owner_assets[sellAsset]  = nil
	else
		self.storage.owner_assets[sellAsset] = balanceAfter
	end
	
	---------------------------------------------------------
	
	
	let eventArg = sellAsset .. "," .. wishBuyAsset .. "," ..tostring(sellAssetAmount)
	emit OnSellOrder(eventArg)
	return "OK"
end

------ owner 撤销指定卖单
--------args: LNK,3000,BTC,2000
function M:cancelSellOrder(args: string)
	checkState(self)
	check_caller_frame_valid(self)
	if self.storage.owner ~= caller_address then
        return error("you have not permission to call this api")
    end

	print(args)

	var ss = parse_args(args,4,"cancelSellOrder args wrong")
	var sellAsset = tostring(ss[1])
	var sellAssetAmount = tointeger(ss[2])
	var wishBuyAsset = tostring(ss[3])
	var wishBuyAssetAmount = tointeger(ss[4])

	var oriOrder = self.storage.sell_orders[sellAsset .. "," .. wishBuyAsset]
	if (oriOrder == nil) or (#oriOrder < 1)  then
		return error("sellOrder not exitst")
	end
	
	-----------------------remove sell order---------------------------------------
	print(oriOrder)
	var jsonstr = ""
	var ordermm:table = {}
	var orderArr:table = {}
	if (oriOrder == nil) or (#oriOrder < 1)  then
		return error("sellOrder not exitst")
	else
		ordermm = totable(json.loads(oriOrder))
		pprint(ordermm)
		print("orignal jsonstr:"..oriOrder)
		print(ordermm["num"])
		print(ordermm["orderArray"])
		orderArr = totable(ordermm["orderArray"])
		
		if(ordermm == nil) or (orderArr == nil) then
			return error("ori order wrong")
		end
		
		var ordernum = tointeger(ordermm["num"])
		var v = ""
		var pos = 0
		var ss
		var findedpos = 0
		var findstr = tostring(sellAssetAmount)..","..tostring(wishBuyAssetAmount)
		for pos,v in ipairs(orderArr) do
			ss = parse_args(v,3,"sellOrder formate wrong")
			if (tointeger(ss[1]) == sellAssetAmount) and (tointeger(ss[2]) == wishBuyAssetAmount) then
				findedpos = pos
			end
		end
		
		if findedpos == 0 then
			return error("no match order")
		end
		table.remove(orderArr,findedpos)

		if (#orderArr) == 0 then
			print(sellAsset .. "," .. wishBuyAsset.." pair has no sellorder now ")
			self.storage.sell_orders[sellAsset .. "," .. wishBuyAsset] = nil
		else
			ordermm["orderArray"] = orderArr
			ordermm["num"] = tointeger(ordermm["num"]) - 1
			jsonstr = json.dumps(ordermm)
			self.storage.sell_orders[sellAsset .. "," .. wishBuyAsset] = jsonstr
		end
		
		self.storage.sell_orders_num = self.storage.sell_orders_num - 1
		----------------------------------------------------------------------
	end

	------------------modify owner assets--------------------
	adjustAssetAmount(self.storage.owner_assets,sellAsset,sellAssetAmount)
	
	let eventArg = sellAsset .. "," .. wishBuyAsset .. "," ..tostring(sellAssetAmount) .. "," .. tostring(wishBuyAssetAmount)
	emit SellOrderCanceled(eventArg)
	return "OK"
end



return M
