//	FIXME: CLOCKS no hay	currentTime = (int) clock() / CLOCKS_PER_SECOND;
/*				if (currentTime - previousTime > udp_period) {
					nat64_st_delete(udp_st, udp_default, currentTime);
					//Deletes any record whose "lifetime" has exceeded "udp_default"
					previousTime = currentTime;
				}
*/				//Querying the UDP BIB
				bib_entry = nat64_bib_select(udp_bib, &(inner->src.u3.in6), inner->src.u.udp.port);
				if (bib_entry == NULL) {
					pr_debug("FIRST O");
					//Allocate memory
					ipv6_ta = (struct nat64_ipv6_ta *) kmalloc(sizeof(struct nat64_ipv6_ta), GFP_KERNEL);
					if (ipv6_ta != NULL) {
						//Initialize IPv6 t.a. structure
						nat64_initialize_ipv6_ta(ipv6_ta, &(inner->src.u3.in6), inner->src.u.udp.port);
						//Verify if there's an address available in the IPv4 pool
						//ipv4_pool_ta = nat64_ipv4_pool_address_available(ipv6_ta);	
						//if (ipv4_pool_ta != NULL) {
							//Allocate memory for BIB entry
							bib_entry = (struct nat64_bib_entry *) kmalloc(sizeof(struct nat64_bib_entry), GFP_KERNEL);
							//Allocate memory for ST entry
							st_entry = (struct nat64_st_entry *) kmalloc(sizeof(struct nat64_st_entry), GFP_KERNEL);
							if (bib_entry != NULL && st_entry != NULL) {
								//Initialize BIB entry
								nat64_initialize_bib_entry(bib_entry, 
									&(inner->src.u3.in6), 
									inner->src.u.udp.port, 
									ip4srcaddr, //&(ipv4_pool_ta->ip4a), 
									new_port);//ipv4_pool_ta->port);
								//Insert entry into UDP BIB
								nat64_bib_insert(udp_bib, bib_entry);
								//Initialize ST entry
								nat64_initialize_st_entry(st_entry,
									&(inner->src.u3.in6), inner->src.u.udp.port,
									&(inner->dst.u3.in6), inner->dst.u.udp.port,
									ip4srcaddr, new_port, //&(ipv4_pool_ta->ip4a), ipv4_pool_ta->port,
									&(inner->dst.u3.in), inner->dst.u.udp.port,
									currentTime);
									//Insert entry into UDP ST
									nat64_st_insert(udp_st, st_entry);
									res = true;
									goto end;
							} else {
								//FIXME: bib_entry = NULL;
								//FIXME: st_entry = NULL;
								kfree(bib_entry);
								kfree(st_entry);
							}
						//}
					}
				} else {
					//Querying the UDP ST
					pr_debug("SECOND O");
					st_entry = nat64_st_select(udp_st, &(bib_entry->ta_4.ip4a),
						bib_entry->ta_4.port, &(inner->dst.u3.in), inner->dst.u.udp.port);
					if (st_entry != NULL) {
						nat64_st_update(udp_st, &(bib_entry->ta_4.ip4a),
						bib_entry->ta_4.port, &(inner->dst.u3.in),
						inner->dst.u.udp.port, currentTime);
						res = true;
						goto end;
					} else {
						//Allocate memory for ST entry
						st_entry = (struct nat64_st_entry *) kmalloc(sizeof(struct nat64_st_entry), GFP_KERNEL);
						if (st_entry != NULL) {
							//Initialize ST entry
							nat64_initialize_st_entry(st_entry,
								&(inner->src.u3.in6), inner->src.u.udp.port,
								&(inner->dst.u3.in6), inner->dst.u.udp.port,
								ip4srcaddr, new_port, //&(ipv4_pool_ta->ip4a), ipv4_pool_ta->port,
								&(inner->dst.u3.in), inner->dst.u.udp.port,
								currentTime);
							//Insert entry into UDP ST
							nat64_st_insert(udp_st, st_entry);
							res = true;
							goto end;
						}
					}
				}
